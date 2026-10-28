#pragma once

#include <QDBusConnection>
#include <QObject>
#include <QString>

namespace greylock {

/// Keeps one copy of the app per session: a second launch asks the first to show its window,
/// then exits, so reminders never come twice.
class SingleInstance : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.greylocklabs.SingleInstance")

public:
    /// `service` is the bus name the running copy holds, such as the app's id.
    SingleInstance(const QString &service, const QDBusConnection &bus, QObject *parent = nullptr);

    /// Takes the bus name. When another instance holds it, asks that one to
    /// activate, or to open `uris` when there are any, and returns false.
    /// Without a session bus there is nobody to ask, so this instance runs.
    bool claim(const QStringList &uris = {});

public Q_SLOTS:
    Q_SCRIPTABLE void Activate();
    Q_SCRIPTABLE void Open(const QStringList &uris);

Q_SIGNALS:
    void activated();
    void opened(const QStringList &uris);

private:
    QString m_service;
    QDBusConnection m_bus;
};

} // namespace greylock
