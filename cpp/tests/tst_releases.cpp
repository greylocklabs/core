#include "greylock/Releases.h"

#include <QFile>
#include <QTemporaryDir>
#include <QTest>

using namespace greylock;
using namespace Qt::StringLiterals;

class TestReleases : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void apiIsTheAppsRepository();
    void parsesPublishedReleases();
    void listIsNewestFirst();
    void systemsWithoutPackagesSayWhy_data();
    void systemsWithoutPackagesSayWhy();
    void findsTheSystemsPackage();

private:
    static QJsonObject release(const QString &tag, const QStringList &assets = {});
};

QJsonObject TestReleases::release(const QString &tag, const QStringList &assets)
{
    QJsonArray files;
    for (const QString &name : assets)
        files.append(QJsonObject{{u"name"_s, name},
                                 {u"browser_download_url"_s, u"https://example.com/"_s + name},
                                 {u"digest"_s, u"sha256:abc"_s}});
    return {{u"tag_name"_s, tag},
            {u"html_url"_s, u"https://example.com/"_s + tag},
            {u"assets"_s, files}};
}

void TestReleases::apiIsTheAppsRepository()
{
    QCOMPARE(releases::defaultApi(), QUrl(u"https://api.github.com/repos/greylocklabs/demo"_s));
}

void TestReleases::parsesPublishedReleases()
{
    const std::optional<Release> parsed = releases::parse(release(u"v1.2.0"_s, {u"a.rpm"_s}));
    QVERIFY(parsed);
    QCOMPARE(parsed->version, QVersionNumber(1, 2, 0));
    QCOMPARE(parsed->tag, u"v1.2.0"_s);
    QCOMPARE(parsed->page, QUrl(u"https://example.com/v1.2.0"_s));
    QCOMPARE(parsed->assets.size(), 1);
    QCOMPARE(parsed->assets.first().sha256, u"abc"_s);

    QJsonObject draft = release(u"v1.3.0"_s);
    draft[u"draft"_s] = true;
    QVERIFY(!releases::parse(draft));
    QJsonObject prerelease = release(u"v1.3.0"_s);
    prerelease[u"prerelease"_s] = true;
    QVERIFY(!releases::parse(prerelease));
    QVERIFY(!releases::parse(release(u"nightly"_s)));
}

void TestReleases::listIsNewestFirst()
{
    const QList<Release> list =
        releases::parseList({release(u"v1.2.0"_s), release(u"nightly"_s), release(u"v1.10.0"_s)});
    QCOMPARE(list.size(), 2);
    QCOMPARE(list.first().version, QVersionNumber(1, 10, 0));
}

void TestReleases::systemsWithoutPackagesSayWhy_data()
{
    QTest::addColumn<QByteArray>("osRelease");
    QTest::addColumn<QString>("architecture");
    QTest::addColumn<int>("kind");
    QTest::addColumn<QString>("why");

    const auto rpm = int(PackageSystem::Kind::Rpm);
    const auto deb = int(PackageSystem::Kind::Deb);
    const auto none = int(PackageSystem::Kind::Unsupported);
    QTest::newRow("fedora 44") << QByteArray("ID=fedora\nVERSION_ID=44\n") << u"x86_64"_s << rpm
                               << QString();
    QTest::newRow("fedora 45") << QByteArray("ID=fedora\nVERSION_ID=45\n") << u"x86_64"_s << rpm
                               << QString();
    QTest::newRow("fedora 43") << QByteArray("ID=fedora\nVERSION_ID=43\nPRETTY_NAME='Fedora 43'\n")
                               << u"x86_64"_s << none << u"Fedora 43 is older than Fedora 44"_s;
    QTest::newRow("fedora rawhide") << QByteArray("ID=fedora\nVERSION_ID=rawhide\n") << u"x86_64"_s
                                    << none << u"cannot read the Fedora version"_s;
    QTest::newRow("debian 13") << QByteArray("ID=debian\nVERSION_ID=\"13\"\n") << u"x86_64"_s << deb
                               << QString();
    QTest::newRow("debian testing") << QByteArray("ID=debian\nPRETTY_NAME=\"Debian forky/sid\"\n")
                                    << u"x86_64"_s << deb << QString();
    QTest::newRow("debian 12") << QByteArray("ID=debian\nVERSION_ID=12\nPRETTY_NAME=Debian 12\n")
                               << u"x86_64"_s << none << u"older than Debian 13"_s;
    QTest::newRow("ubuntu 26.04") << QByteArray("ID=ubuntu\nVERSION_ID=\"26.04\"\n") << u"x86_64"_s
                                  << deb << QString();
    QTest::newRow("ubuntu 24.04") << QByteArray("ID=ubuntu\nVERSION_ID=24.04\n") << u"x86_64"_s
                                  << none << u"older than Ubuntu 26.04: Demo needs Qt 6.8"_s;
    QTest::newRow("arch") << QByteArray("ID=arch\nPRETTY_NAME=\"Arch Linux\"\n") << u"x86_64"_s
                          << none << u"Arch Linux is not one Demo ships packages for"_s;
    QTest::newRow("arm") << QByteArray("ID=fedora\nVERSION_ID=44\n") << u"arm64"_s << none
                         << u"no packages for arm64"_s;
    QTest::newRow("unknown") << QByteArray() << u"x86_64"_s << none << u"is missing"_s;
}

void TestReleases::systemsWithoutPackagesSayWhy()
{
    QFETCH(QByteArray, osRelease);
    QFETCH(QString, architecture);
    QFETCH(int, kind);
    QFETCH(QString, why);
    QTemporaryDir dir;
    QString path = dir.filePath(u"missing"_s);
    if (!osRelease.isEmpty()) {
        QFile file(dir.filePath(u"os-release"_s));
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write(osRelease);
        path = file.fileName();
    }
    const PackageSystem system = PackageSystem::detect(path, architecture);
    QCOMPARE(int(system.kind), kind);
    // Demo builds no debs for Ubuntu of its own, so Ubuntu takes Debian's.
    QVERIFY(!system.ubuntu);
    QVERIFY2(why.isEmpty() ? system.why.isEmpty() : system.why.contains(why),
             qPrintable(system.why));
}

void TestReleases::findsTheSystemsPackage()
{
    const Release published = *releases::parse(release(
        u"v1.2.0"_s, {u"demo-1.2.0.tar.xz"_s, u"demo-1.2.0-1.fc44.x86_64.rpm"_s,
                      u"demo-debuginfo-1.2.0-1.fc44.x86_64.rpm"_s, u"demo_1.2.0-1_amd64.deb"_s,
                      u"demo-ubuntu_1.2.0-1_amd64.deb"_s, u"other_1.2.0-1_amd64.deb"_s}));
    PackageSystem system;
    system.kind = PackageSystem::Kind::Rpm;
    QCOMPARE(system.packageIn(published)->name, u"demo-1.2.0-1.fc44.x86_64.rpm"_s);
    system.kind = PackageSystem::Kind::Deb;
    QCOMPARE(system.packageIn(published)->name, u"demo_1.2.0-1_amd64.deb"_s);
    system.ubuntu = true;
    QCOMPARE(system.packageIn(published)->name, u"demo-ubuntu_1.2.0-1_amd64.deb"_s);
    QVERIFY(
        !system.packageIn(*releases::parse(release(u"v1.2.0"_s, {u"other_1.2.0-1_amd64.deb"_s}))));
    system.kind = PackageSystem::Kind::Unsupported;
    QVERIFY(!system.packageIn(published));
}

QTEST_GUILESS_MAIN(TestReleases)
#include "tst_releases.moc"
