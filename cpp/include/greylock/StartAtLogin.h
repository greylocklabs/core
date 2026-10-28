#pragma once

#include <QObject>
#include <QPointer>
#include <QVariantMap>

namespace greylock {

class Autostart;

/// The "start when I log in" setting. It only makes sense while the app keeps running after
/// its window closes, so the app turns it off when that is turned off. An app shows it to QML
/// through a singleton of its own.
class StartAtLogin : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY changed)
    /// False for sample data and screenshots, which never touch the session.
    Q_PROPERTY(bool available READ available NOTIFY changed)
    Q_PROPERTY(QString error READ error NOTIFY changed)

public:
    static StartAtLogin *instance();
    /// Starts managing `autostart`; until then, as in tests and screenshots, it is unavailable.
    void setup(Autostart *autostart);

    [[nodiscard]] bool enabled() const;
    void setEnabled(bool enabled);
    [[nodiscard]] bool available() const { return m_autostart != nullptr; }
    [[nodiscard]] QString error() const { return m_error; }

    /// The desktop portal asked in a Flatpak, which tests stand in for.
    void setPortal(const QString &service) { m_portal = service; }

Q_SIGNALS:
    void changed();

private:
    explicit StartAtLogin(QObject *parent);
    /// Asks the Background portal to start the app at login, or not. A refusal
    /// takes the setting back and says why.
    void requestBackground(bool autostart);
    Q_SLOT void backgroundAnswered(uint response, const QVariantMap &results);
    void refused(const QString &why);

    Autostart *m_autostart = nullptr;
    QString m_error;
    QString m_portal = QStringLiteral("org.freedesktop.portal.Desktop");
    /// The portal request waiting for its answer, by its object path.
    QString m_request;
    bool m_requestedAutostart = false;
};

} // namespace greylock
