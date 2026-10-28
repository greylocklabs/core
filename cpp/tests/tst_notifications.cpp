#include "greylock/Notifications.h"

#include <QFile>
#include <QTemporaryDir>
#include <QTest>

using namespace greylock;
using namespace Qt::StringLiterals;

class TestNotifications : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void keepsTheLogoWhereTheDaemonCanReadIt();
};

void TestNotifications::keepsTheLogoWhereTheDaemonCanReadIt()
{
    QTemporaryDir dir;
    const QString folder = dir.filePath(u"cache/demo"_s);
    const QString path = cachedLogo(folder, "first");
    QCOMPARE(path, folder + u"/notification-icon.png"_s);
    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QCOMPARE(file.readAll(), QByteArray("first"));
    file.close();
    // A new logo replaces the old one; the same one is left as it is.
    QCOMPARE(cachedLogo(folder, "second"), path);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QCOMPARE(file.readAll(), QByteArray("second"));
    // Somewhere it cannot be written gives nothing.
    QFile blocker(dir.filePath(u"blocked"_s));
    QVERIFY(blocker.open(QIODevice::WriteOnly));
    blocker.close();
    QCOMPARE(cachedLogo(dir.filePath(u"blocked/demo"_s), "logo"), QString());
}

QTEST_GUILESS_MAIN(TestNotifications)
#include "tst_notifications.moc"
