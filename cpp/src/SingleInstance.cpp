#include "greylock/SingleInstance.h"

#include <QDBusConnectionInterface>
#include <QDBusError>
#include <QDBusMessage>
#include <QLoggingCategory>

using namespace Qt::StringLiterals;

namespace greylock {

namespace {

Q_LOGGING_CATEGORY(lcSession, GREYLOCK_APP_DIR ".session", QtInfoMsg)

const QString kPath = u"/org/greylocklabs/SingleInstance"_s;
const QString kInterface = u"org.greylocklabs.SingleInstance"_s;

} // namespace

SingleInstance::SingleInstance(const QString &service, const QDBusConnection &bus, QObject *parent)
    : QObject(parent), m_service(service), m_bus(bus)
{}

bool SingleInstance::claim(const QStringList &uris)
{
    if (!m_bus.isConnected())
        return true;
    // Exported before the name is taken, so a launch that finds the name can
    // always reach the object.
    if (!m_bus.registerObject(kPath, this, QDBusConnection::ExportScriptableSlots)) {
        qCWarning(lcSession) << "could not export the activation object:"
                             << m_bus.lastError().message();
        return true;
    }
    const auto reply =
        m_bus.interface()->registerService(m_service, QDBusConnectionInterface::DontQueueService,
                                           QDBusConnectionInterface::DontAllowReplacement);
    if (reply.isValid() && reply.value() == QDBusConnectionInterface::ServiceRegistered)
        return true;
    m_bus.unregisterObject(kPath);
    QDBusMessage call = QDBusMessage::createMethodCall(m_service, kPath, kInterface,
                                                       uris.isEmpty() ? u"Activate"_s : u"Open"_s);
    if (!uris.isEmpty())
        call << uris;
    const QDBusMessage answer = m_bus.call(call, QDBus::BlockWithGui, 5000);
    if (answer.type() == QDBusMessage::ErrorMessage) {
        // The other instance is stuck or gone; better two than none.
        qCWarning(lcSession) << "could not reach the copy already running:"
                             << answer.errorMessage();
        return true;
    }
    return false;
}

void SingleInstance::Activate()
{
    Q_EMIT activated();
}

void SingleInstance::Open(const QStringList &uris)
{
    Q_EMIT activated();
    Q_EMIT opened(uris);
}

} // namespace greylock
