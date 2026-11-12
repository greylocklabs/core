#include "greylock/Translations.h"

#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>

using namespace Qt::StringLiterals;

namespace greylock {

void installTranslations(QCoreApplication &app)
{
    // The app's own name, not its folder's, which a development build changes.
    const QString name = QStringLiteral(GREYLOCK_APP_DIR);
    auto *words = new QTranslator(&app);
    if (words->load(QLocale(), name, u"_"_s, u":/i18n"_s) ||
        words->load(name + u"_en"_s, u":/i18n"_s))
        QCoreApplication::installTranslator(words);
    else
        delete words;
}

} // namespace greylock
