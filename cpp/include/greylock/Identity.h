#pragma once

#include <QString>

/// Who this build of the app is. A development build has its own app id, folders and keyring
/// entries, so it runs beside an installed copy without sharing its data or settings.
namespace greylock::identity {

/// What a build is called, worked out from whether it is a development build.
struct Names
{
    /// The app's id, such as "org.example.Demo", or "org.example.Demo.Devel".
    QString appId;
    /// The folder under each XDG directory, such as "demo" or "demo-devel".
    QString dirName;
    /// What the desktop shows, such as "Demo" or "Demo Devel".
    QString displayName;
};
[[nodiscard]] Names namesFor(bool devel);

/// This build's names.
[[nodiscard]] QString appId();
[[nodiscard]] QString dirName();
[[nodiscard]] QString displayName();
[[nodiscard]] bool isDevel();
/// Whether the app runs in a Flatpak sandbox, where Flathub delivers updates and the host is
/// reached through portals. <env>_FLATPAK=0 or 1 decides instead, as tests need.
[[nodiscard]] bool isFlatpak();

} // namespace greylock::identity
