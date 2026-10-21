#include "greylock/App.h"
#include "greylock/Identity.h"

#include <QTest>

using namespace greylock;
using namespace Qt::StringLiterals;

class TestIdentity : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void theAppIsWhatCMakeSays();
    void developmentBuildsStandApart();
    void flatpakCanBeForced();
};

void TestIdentity::theAppIsWhatCMakeSays()
{
    // Built on its own, core is built for a made-up Demo app.
    QCOMPARE(app().id, u"org.example.Demo"_s);
    QCOMPARE(app().env, u"DEMO"_s);
    QVERIFY(!identity::isDevel());
    QCOMPARE(identity::appId(), u"org.example.Demo"_s);
    QCOMPARE(identity::dirName(), u"demo"_s);
}

void TestIdentity::developmentBuildsStandApart()
{
    // An installed app and a development build never share a name or folder.
    const identity::Names devel = identity::namesFor(true);
    QCOMPARE(devel.appId, u"org.example.Demo.Devel"_s);
    QCOMPARE(devel.dirName, u"demo-devel"_s);
    QCOMPARE(devel.displayName, u"Demo Devel"_s);
}

void TestIdentity::flatpakCanBeForced()
{
    qputenv("DEMO_FLATPAK", "1");
    QVERIFY(identity::isFlatpak());
    qputenv("DEMO_FLATPAK", "0");
    QVERIFY(!identity::isFlatpak());
    qunsetenv("DEMO_FLATPAK");
}

QTEST_GUILESS_MAIN(TestIdentity)
#include "tst_identity.moc"
