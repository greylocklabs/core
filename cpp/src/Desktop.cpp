#include "greylock/Desktop.h"

#include "greylock/Identity.h"

#include <QtGlobal>

namespace greylock::desktop {

void useDesktopFileDialogs()
{
    if (!qEnvironmentVariableIsEmpty("QT_QPA_PLATFORMTHEME") || identity::isFlatpak())
        return;
    // Qt's portal theme draws everything else as the desktop's own theme does, and falls back
    // to Qt's dialogs where no portal answers.
    qputenv("QT_QPA_PLATFORMTHEME", "xdgdesktopportal");
}

} // namespace greylock::desktop
