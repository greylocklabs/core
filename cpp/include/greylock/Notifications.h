#pragma once

#include <QByteArray>
#include <QObject>
#include <QString>
#include <QStringList>

#include <functional>

namespace greylock {

/// A copy of `logo` in `dir`, for daemons that read icons from files: written
/// when missing or different, so a new logo replaces an old one. Empty when it
/// cannot be written.
[[nodiscard]] QString cachedLogo(const QString &dir, const QByteArray &logo);

/// Somewhere to show desktop notifications.
class NotificationServer : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

    /// Shows a notification; `actions` alternates keys and their labels, and
    /// the key "default" is a click on the notification itself. `shown` gets
    /// the notification's id, or 0 if it could not be shown.
    virtual void show(const QString &title, const QString &body, const QStringList &actions,
                      std::function<void(uint id)> shown) = 0;
    virtual void close(uint id) = 0;

Q_SIGNALS:
    void actionInvoked(uint id, const QString &key);
    void closed(uint id);
};

/// The desktop's notification daemon, over org.freedesktop.Notifications.
class FreedesktopNotifications : public NotificationServer
{
    Q_OBJECT

public:
    /// `logo` is the app's logo as a resource, shown when its icon is not installed, as when it
    /// runs from a build; `category` tells the daemon what the notifications are about, such as
    /// "x-gnome.calendar", or nothing.
    explicit FreedesktopNotifications(QString logo, QString category = {},
                                      QObject *parent = nullptr);

    void show(const QString &title, const QString &body, const QStringList &actions,
              std::function<void(uint id)> shown) override;
    void close(uint id) override;

private Q_SLOTS:
    void onActionInvoked(uint id, const QString &key);
    void onClosed(uint id, uint reason);

private:
    [[nodiscard]] QString appIcon() const;

    QString m_logo;
    QString m_category;
};

} // namespace greylock
