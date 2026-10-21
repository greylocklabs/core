#include "greylock/Identity.h"

#include "greylock/App.h"

#include <QFileInfo>

using namespace Qt::StringLiterals;

namespace greylock::identity {

Names namesFor(bool devel)
{
    const App &a = app();
    if (devel)
        return {a.id + u".Devel"_s, a.dirName + u"-devel"_s, a.name + u" Devel"_s};
    return {a.id, a.dirName, a.name};
}

QString appId()
{
    return namesFor(isDevel()).appId;
}

QString dirName()
{
    return namesFor(isDevel()).dirName;
}

QString displayName()
{
    return namesFor(isDevel()).displayName;
}

bool isDevel()
{
    return app().devel;
}

bool isFlatpak()
{
    const QByteArray forced = qgetenv(QString(app().env + u"_FLATPAK"_s).toUtf8().constData());
    if (!forced.isEmpty())
        return forced != "0";
    return QFileInfo::exists(u"/.flatpak-info"_s);
}

} // namespace greylock::identity
