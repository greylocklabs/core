#include "greylock/Notifications.h"

#include "greylock/Identity.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QVariantMap>

using namespace Qt::StringLiterals;

namespace greylock {

namespace {

Q_LOGGING_CATEGORY(lcSession, GREYLOCK_APP_DIR ".session", QtInfoMsg)

const QString kService = u"org.freedesktop.Notifications"_s;
const QString kPath = u"/org/freedesktop/Notifications"_s;
const QString kInterface = u"org.freedesktop.Notifications"_s;

} // namespace

QString cachedLogo(const QString &dir, const QByteArray &logo)
{
    const QString path = dir + u"/notification-icon.png"_s;
    QFile cached(path);
    if (cached.open(QIODevice::ReadOnly) && cached.readAll() == logo)
        return path;
    cached.close();
    QDir().mkpath(dir);
    QFile out(path);
    if (out.open(QIODevice::WriteOnly | QIODevice::Truncate) && out.write(logo) == logo.size())
        return path;
    return {};
}

// The icon theme's icon when the app is installed; otherwise, as when it runs from a build,
// its bundled logo copied where the daemon can read it.
QString FreedesktopNotifications::appIcon() const
{
    if (QIcon::hasThemeIcon(identity::appId()))
        return identity::appId();
    QFile logo(m_logo);
    if (!logo.open(QIODevice::ReadOnly))
        return identity::appId();
    const QString path =
        cachedLogo(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + u'/' +
                       identity::dirName(),
                   logo.readAll());
    return path.isEmpty() ? identity::appId() : path;
}

FreedesktopNotifications::FreedesktopNotifications(QString logo, QString category, QObject *parent)
    : NotificationServer(parent), m_logo(std::move(logo)), m_category(std::move(category))
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    bus.connect(kService, kPath, kInterface, u"ActionInvoked"_s, this,
                SLOT(onActionInvoked(uint, QString)));
    bus.connect(kService, kPath, kInterface, u"NotificationClosed"_s, this,
                SLOT(onClosed(uint, uint)));
}

void FreedesktopNotifications::show(const QString &title, const QString &body,
                                    const QStringList &actions, std::function<void(uint)> shown)
{
    QDBusMessage call = QDBusMessage::createMethodCall(kService, kPath, kInterface, u"Notify"_s);
    QVariantMap hints{
        {u"desktop-entry"_s, greylock::identity::appId()},
        // Normal urgency; the daemon decides how long it stays.
        {u"urgency"_s, QVariant::fromValue(uchar(1))},
    };
    if (!m_category.isEmpty())
        hints.insert(u"category"_s, m_category);
    call << greylock::identity::displayName() << uint(0) << appIcon() << title << body << actions
         << hints << int(-1);
    auto *watcher =
        new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(call), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [shown = std::move(shown)](QDBusPendingCallWatcher *watcher) {
                const QDBusPendingReply<uint> reply = *watcher;
                watcher->deleteLater();
                if (reply.isError())
                    qCWarning(lcSession)
                        << "could not show a notification:" << reply.error().message();
                shown(reply.isError() ? 0 : reply.value());
            });
}

void FreedesktopNotifications::close(uint id)
{
    QDBusConnection::sessionBus().asyncCall(
        QDBusMessage::createMethodCall(kService, kPath, kInterface, u"CloseNotification"_s) << id);
}

void FreedesktopNotifications::onActionInvoked(uint id, const QString &key)
{
    Q_EMIT actionInvoked(id, key);
}

void FreedesktopNotifications::onClosed(uint id, uint reason)
{
    Q_UNUSED(reason)
    Q_EMIT closed(id);
}

} // namespace greylock
