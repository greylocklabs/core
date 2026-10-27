#pragma once

/// How the app fits into the desktop it runs on.
namespace greylock::desktop {

/// Called before the application is made: file and folder dialogs come from the desktop's
/// file chooser portal, with its places and bookmarks, rather than Qt's own. Qt's dialog is
/// what an app without widgets gets otherwise, as KDE's needs them. A platform theme the user
/// chose is left alone, and so is a Flatpak, which uses the portal already.
void useDesktopFileDialogs();

} // namespace greylock::desktop
