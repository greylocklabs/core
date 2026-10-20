#pragma once

#include <QByteArray>
#include <QByteArrayView>
#include <QString>

#include <optional>

namespace greylock::base16 {

/// A theme file, as TOML, from a base16 or base24 scheme in the classic
/// layout or tinted-theming's, with colors nudged where text would not read.
/// Empty, with `error` set, when the scheme lacks a name or a color.
[[nodiscard]] std::optional<QByteArray> toTheme(QByteArrayView yaml, QString *error);

/// Whether a file is a base16 scheme rather than a theme file, by its name.
[[nodiscard]] bool isScheme(const QString &path);

} // namespace greylock::base16
