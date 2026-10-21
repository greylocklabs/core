#pragma once

#include <QString>

namespace greylock {

/// The app core is built into, as its CMake set GREYLOCK_APP_ID, _NAME, _DIR, _ENV and
/// _DEVEL before add_subdirectory(core). Core is built inside each app, so these are fixed
/// when the app is built and nothing sets them at startup.
struct App
{
    /// The release build's reverse-DNS id, such as "org.example.Demo".
    QString id;
    /// What the desktop shows, such as "Demo".
    QString name;
    /// The folder under each XDG directory, such as "demo".
    QString dirName;
    /// The prefix of the app's environment overrides, such as "DEMO".
    QString env;
    /// A development build, which takes its own identity; see identity::namesFor().
    bool devel = false;
};

[[nodiscard]] const App &app();

} // namespace greylock
