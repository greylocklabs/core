#include "greylock/Issues.h"

#include "greylock/App.h"
#include "greylock/Identity.h"

#include <QUrlQuery>

#include <algorithm>

using namespace Qt::StringLiterals;

namespace greylock::issues {

QString shortenedMark()
{
    return u"\n[shortened; run `%1 doctor` for the rest]"_s.arg(identity::command());
}

QUrl fitUrl(const std::function<QUrl(const QString &)> &build, const QString &text,
            const QString &mark)
{
    // The limit is on the encoded URL, where a space or newline takes three characters, so the
    // text is cut until the encoded form fits.
    QUrl url = build(text);
    QString body = text;
    while (url.toString(QUrl::FullyEncoded).size() > kMaxUrlLength && !body.isEmpty()) {
        const qsizetype over = url.toString(QUrl::FullyEncoded).size() - kMaxUrlLength;
        body.chop(std::max<qsizetype>(over / 3, 64));
        url = build(body + mark);
    }
    return url;
}

QUrl newBug(const QString &report)
{
    return fitUrl(
        [](const QString &body) {
            QUrl url(u"https://github.com/%1/issues/new"_s.arg(app().repo));
            QUrlQuery query;
            query.addQueryItem(u"template"_s, u"bug_report.yml"_s);
            query.addQueryItem(u"diagnostics"_s, body);
            url.setQuery(query);
            return url;
        },
        report, shortenedMark());
}

} // namespace greylock::issues
