#include "greylock/Tray.h"

#include "greylock/Identity.h"

#include <QCoreApplication>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusServiceWatcher>
#include <QIcon>
#include <QImage>
#include <QLoggingCategory>
#include <QtEndian>

using namespace Qt::StringLiterals;

namespace greylock {

namespace {

Q_LOGGING_CATEGORY(lcSession, GREYLOCK_APP_DIR ".session", QtInfoMsg)

const QString kItemPath = u"/StatusNotifierItem"_s;
const QString kMenuPath = u"/MenuBar"_s;

void registerTypes()
{
    static const bool done = [] {
        qDBusRegisterMetaType<DBusMenuEntry>();
        qDBusRegisterMetaType<QList<DBusMenuProperties>>();
        qDBusRegisterMetaType<DBusMenuProperties>();
        qDBusRegisterMetaType<DBusTrayImage>();
        qDBusRegisterMetaType<QList<DBusTrayImage>>();
        qDBusRegisterMetaType<DBusTrayToolTip>();
        return true;
    }();
    Q_UNUSED(done);
}

// The bundled logo at the sizes panels ask for, for hosts without the app's icon in their
// theme, as when it runs from a build.
QList<DBusTrayImage> logoImages(const QString &path)
{
    const QImage logo(path);
    QList<DBusTrayImage> images;
    if (logo.isNull())
        return images;
    for (int size : {22, 32, 48}) {
        const QImage image = logo.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation)
                                 .convertToFormat(QImage::Format_ARGB32);
        DBusTrayImage tray{image.width(), image.height(), {}};
        tray.pixels.resize(qsizetype(image.width()) * image.height() * 4);
        auto *out = reinterpret_cast<quint32 *>(tray.pixels.data());
        for (int y = 0; y < image.height(); ++y) {
            const auto *line = reinterpret_cast<const quint32 *>(image.constScanLine(y));
            for (int x = 0; x < image.width(); ++x)
                *out++ = qToBigEndian(line[x]);
        }
        images.append(tray);
    }
    return images;
}

} // namespace

QDBusArgument &operator<<(QDBusArgument &argument, const DBusMenuEntry &entry)
{
    argument.beginStructure();
    argument << entry.id << entry.properties;
    argument.beginArray(QMetaType::fromType<QDBusVariant>());
    for (const QVariant &child : entry.children)
        argument << QDBusVariant(child);
    argument.endArray();
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, DBusMenuEntry &entry)
{
    argument.beginStructure();
    argument >> entry.id >> entry.properties;
    entry.children.clear();
    argument.beginArray();
    while (!argument.atEnd()) {
        QDBusVariant child;
        argument >> child;
        entry.children.append(child.variant());
    }
    argument.endArray();
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const DBusMenuProperties &properties)
{
    argument.beginStructure();
    argument << properties.id << properties.properties;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, DBusMenuProperties &properties)
{
    argument.beginStructure();
    argument >> properties.id >> properties.properties;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const DBusTrayImage &image)
{
    argument.beginStructure();
    argument << image.width << image.height << image.pixels;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, DBusTrayImage &image)
{
    argument.beginStructure();
    argument >> image.width >> image.height >> image.pixels;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const DBusTrayToolTip &toolTip)
{
    argument.beginStructure();
    argument << toolTip.iconName << toolTip.images << toolTip.title << toolTip.text;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, DBusTrayToolTip &toolTip)
{
    argument.beginStructure();
    argument >> toolTip.iconName >> toolTip.images >> toolTip.title >> toolTip.text;
    argument.endStructure();
    return argument;
}

TrayStatusItem::TrayStatusItem(const QString &logo, QObject *parent) : QObject(parent)
{
    if (QIcon::hasThemeIcon(greylock::identity::appId()))
        m_iconName = greylock::identity::appId();
    else
        m_iconPixmap = logoImages(logo);
}

QString TrayStatusItem::id() const
{
    return greylock::identity::dirName();
}

QString TrayStatusItem::title() const
{
    return greylock::identity::displayName();
}

DBusTrayToolTip TrayStatusItem::toolTip() const
{
    return {m_iconName, m_iconPixmap, title(), {}};
}

QDBusObjectPath TrayStatusItem::menu() const
{
    return QDBusObjectPath(kMenuPath);
}

void TrayStatusItem::Activate(int, int)
{
    Q_EMIT activated();
}

void TrayStatusItem::ProvideXdgActivationToken(const QString &token)
{
    // Qt's Wayland support hands it over with the window's next requestActivate().
    qputenv("XDG_ACTIVATION_TOKEN", token.toUtf8());
}

void TrayStatusItem::SecondaryActivate(int, int)
{
    Q_EMIT activated();
}

void TrayStatusItem::ContextMenu(int, int)
{
    // Hosts show the menu at Menu themselves; this is for those that cannot.
}

void TrayStatusItem::Scroll(int, const QString &) {}

void TrayMenu::setActions(const QList<TrayAction> &actions)
{
    m_actions = actions;
    Q_EMIT LayoutUpdated(++m_revision, 0);
}

QVariantMap TrayMenu::propertiesOf(int id) const
{
    if (id == 0)
        return {{u"children-display"_s, u"submenu"_s}};
    if (id < 1 || id > m_actions.size())
        return {};
    const TrayAction &action = m_actions.at(id - 1);
    if (action.key.isEmpty())
        return {{u"type"_s, u"separator"_s}};
    return {{u"label"_s, action.label}};
}

uint TrayMenu::GetLayout(int parentId, int recursionDepth, const QStringList &,
                         DBusMenuEntry &layout)
{
    layout = {parentId, propertiesOf(parentId), {}};
    // The menu is one level deep: only the root has entries.
    if (parentId == 0 && recursionDepth != 0) {
        for (int id = 1; id <= m_actions.size(); ++id)
            layout.children.append(QVariant::fromValue(DBusMenuEntry{id, propertiesOf(id), {}}));
    }
    return m_revision;
}

QList<DBusMenuProperties> TrayMenu::GetGroupProperties(const QList<int> &ids, const QStringList &)
{
    QList<DBusMenuProperties> result;
    for (int id : ids)
        result.append({id, propertiesOf(id)});
    return result;
}

QDBusVariant TrayMenu::GetProperty(int id, const QString &name)
{
    return QDBusVariant(propertiesOf(id).value(name));
}

void TrayMenu::Event(int id, const QString &eventId, const QDBusVariant &, uint)
{
    if (eventId != u"clicked" || id < 1 || id > m_actions.size() ||
        m_actions.at(id - 1).key.isEmpty())
        return;
    const QString key = m_actions.at(id - 1).key;
    // After the reply, so quitting does not cut it off.
    QMetaObject::invokeMethod(this, [this, key] { Q_EMIT picked(key); }, Qt::QueuedConnection);
}

bool TrayMenu::AboutToShow(int)
{
    return false;
}

TrayItem::TrayItem(const QDBusConnection &bus, const QString &logo, const QString &watcher,
                   QObject *parent)
    : QObject(parent), m_bus(bus), m_watcher(watcher),
      // A Flatpak may not own the usual name, whose process id is the sandbox's
      // anyway, so it offers the entry under its connection's own name.
      m_service(greylock::identity::isFlatpak()
                    ? bus.baseService()
                    : u"org.kde.StatusNotifierItem-%1-1"_s.arg(QCoreApplication::applicationPid())),
      m_item(new TrayStatusItem(logo, this)), m_menu(new TrayMenu(this)),
      m_watching(
          new QDBusServiceWatcher(watcher, bus, QDBusServiceWatcher::WatchForRegistration, this))
{
    registerTypes();
    connect(m_item, &TrayStatusItem::activated, this, &TrayItem::activated);
    connect(m_menu, &TrayMenu::picked, this, &TrayItem::triggered);
    // A panel started, or restarted, after the app learns of the entry anew.
    connect(m_watching, &QDBusServiceWatcher::serviceRegistered, this, [this] {
        if (m_visible)
            announce();
    });
}

void TrayItem::setMenu(const QList<TrayAction> &actions)
{
    m_menu->setActions(actions);
}

TrayItem::~TrayItem()
{
    setVisible(false);
}

void TrayItem::setVisible(bool visible)
{
    if (visible == m_visible || !m_bus.isConnected())
        return;
    m_visible = visible;
    const auto exports = QDBusConnection::ExportScriptableSlots |
                         QDBusConnection::ExportScriptableSignals |
                         QDBusConnection::ExportAllProperties;
    if (visible) {
        m_bus.registerObject(kItemPath, m_item, exports);
        m_bus.registerObject(kMenuPath, m_menu, exports);
        if (!greylock::identity::isFlatpak())
            m_bus.registerService(m_service);
        announce();
    } else {
        if (!greylock::identity::isFlatpak())
            m_bus.unregisterService(m_service);
        m_bus.unregisterObject(kMenuPath);
        m_bus.unregisterObject(kItemPath);
    }
}

void TrayItem::announce()
{
    QDBusMessage call = QDBusMessage::createMethodCall(m_watcher, u"/StatusNotifierWatcher"_s,
                                                       u"org.kde.StatusNotifierWatcher"_s,
                                                       u"RegisterStatusNotifierItem"_s);
    call << m_service;
    // No host, such as plain GNOME, is not an error: there is just no tray.
    m_bus.callWithCallback(call, this, nullptr, nullptr);
    qCDebug(lcSession) << "tray entry offered as" << m_service;
}

} // namespace greylock
