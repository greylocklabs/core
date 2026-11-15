#pragma once

#include <QString>
#include <QUrl>

#include <functional>

/// New issues in the app's GitHub repository, filled in from the app, for when a report is
/// opened in the browser.
namespace greylock::issues {

/// The longest new-issue address GitHub accepts.
inline constexpr qsizetype kMaxUrlLength = 8000;

/// What a report cut to fit ends with: where to find the rest, the app's `doctor` command.
[[nodiscard]] QString shortenedMark();

/// The address `build` makes from `text`, with `text` cut from its end and `mark` added until
/// the address fits in a URL GitHub accepts.
[[nodiscard]] QUrl fitUrl(const std::function<QUrl(const QString &)> &build, const QString &text,
                          const QString &mark);

/// A new bug report in the app's repository with `report` as its diagnostics, shortened to fit.
[[nodiscard]] QUrl newBug(const QString &report);

} // namespace greylock::issues
