#include "greylock/Translations.h"

#include <QCoreApplication>
#include <QFile>
#include <QLocale>
#include <QTest>
#include <QTranslator>

using namespace Qt::StringLiterals;

class TestTranslations : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void installsTheEnglishWording();
};

void TestTranslations::installsTheEnglishWording()
{
    QLocale::setDefault(QLocale(QLocale::German));
    QVERIFY(QFile::exists(u":/i18n/demo_en.qm"_s));
    greylock::installTranslations(*QCoreApplication::instance());
    QStringList ours;
    for (const QTranslator *words : QCoreApplication::instance()->findChildren<QTranslator *>()) {
        if (words->filePath().startsWith(u":/i18n/"_s))
            ours.append(words->filePath());
    }
    // Demo has no German, so English stands in.
    QCOMPARE(ours, QStringList{u":/i18n/demo_en.qm"_s});
    // translations/demo_en.ts rewords one of core's own strings.
    QCOMPARE(QObject::tr("there are no packages for %1 yet").arg(u"riscv64"_s),
             u"Demo has no packages for riscv64 yet"_s);
}

QTEST_GUILESS_MAIN(TestTranslations)
#include "tst_translations.moc"
