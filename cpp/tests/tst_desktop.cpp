#include "greylock/Desktop.h"

#include <QTest>

class TestDesktop : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void init();
    void asksTheDesktopForFileDialogs();
    void leavesAThemeTheUserChose();
    void leavesAFlatpakAsItIs();
};

void TestDesktop::init()
{
    qunsetenv("QT_QPA_PLATFORMTHEME");
    qputenv("DEMO_FLATPAK", "0");
}

void TestDesktop::asksTheDesktopForFileDialogs()
{
    greylock::desktop::useDesktopFileDialogs();
    QCOMPARE(qgetenv("QT_QPA_PLATFORMTHEME"), QByteArray("xdgdesktopportal"));
}

void TestDesktop::leavesAThemeTheUserChose()
{
    qputenv("QT_QPA_PLATFORMTHEME", "gtk3");
    greylock::desktop::useDesktopFileDialogs();
    QCOMPARE(qgetenv("QT_QPA_PLATFORMTHEME"), QByteArray("gtk3"));
}

void TestDesktop::leavesAFlatpakAsItIs()
{
    qputenv("DEMO_FLATPAK", "1");
    greylock::desktop::useDesktopFileDialogs();
    QVERIFY(qEnvironmentVariableIsEmpty("QT_QPA_PLATFORMTHEME"));
}

QTEST_APPLESS_MAIN(TestDesktop)
#include "tst_desktop.moc"
