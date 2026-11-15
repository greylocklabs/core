#include "greylock/Issues.h"

#include <QTest>
#include <QUrlQuery>

using namespace greylock;
using namespace Qt::StringLiterals;

class TestIssues : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void aLongReportIsShortenedToFit();
    void aShortReportIsUntouched();
};

void TestIssues::aLongReportIsShortenedToFit()
{
    // Spaces and newlines triple when encoded, as in a real log tail.
    QString report;
    for (int i = 0; i < 400; ++i)
        report += u"2026-10-05 14:30:21 demo.sync info: synced item number "_s +
                  QString::number(i) + u"\n"_s;

    const QUrl url = issues::newBug(report);

    QVERIFY(url.toString(QUrl::FullyEncoded).size() <= issues::kMaxUrlLength);
    QVERIFY(url.toString(QUrl::FullyEncoded).size() > issues::kMaxUrlLength - 1000);
    const QUrlQuery query(url);
    QCOMPARE(url.toString(QUrl::RemoveQuery), u"https://github.com/greylocklabs/demo/issues/new"_s);
    QCOMPARE(query.queryItemValue(u"template"_s), u"bug_report.yml"_s);
    const QString body = query.queryItemValue(u"diagnostics"_s, QUrl::FullyDecoded);
    QVERIFY(body.startsWith(u"2026-10-05 14:30:21 demo.sync info: synced item number 0\n"_s));
    QVERIFY(body.endsWith(u"run `demo doctor` for the rest]"_s));
}

void TestIssues::aShortReportIsUntouched()
{
    const QUrl url = issues::newBug(u"Demo 0.1.0\nQt 6.11"_s);
    QCOMPARE(QUrlQuery(url).queryItemValue(u"diagnostics"_s, QUrl::FullyDecoded),
             u"Demo 0.1.0\nQt 6.11"_s);
}

QTEST_GUILESS_MAIN(TestIssues)
#include "tst_issues.moc"
