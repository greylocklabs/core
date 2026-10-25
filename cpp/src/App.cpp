#include "greylock/App.h"

namespace greylock {

const App &app()
{
    static const App app{QStringLiteral(GREYLOCK_APP_ID),
                         QStringLiteral(GREYLOCK_APP_NAME),
                         QStringLiteral(GREYLOCK_APP_DIR),
                         QStringLiteral(GREYLOCK_APP_ENV),
                         QStringLiteral(GREYLOCK_APP_REPO),
                         GREYLOCK_APP_UBUNTU_DEBS,
                         GREYLOCK_APP_DEVEL};
    return app;
}

} // namespace greylock
