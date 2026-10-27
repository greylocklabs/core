# Changelog

What changed in each release of core, for the people working on the apps that use it. Notes collect under Unreleased,
and `scripts/release.sh` dates them.

## 0.8.0 - 2026-10-27

### Added

- `greylock::desktop::useDesktopFileDialogs()`, for an app to call before it makes its application:
  file and folder dialogs come from the desktop's file chooser portal, as KDE's or GNOME's own
  dialog with their places and bookmarks, rather than Qt's. A platform theme the user set, or a
  Flatpak, is left as it is.

## 0.7.0 - 2026-10-26

### Added

- `tooling/release.sh` and `check-version.sh` release Android apps too, which keep their version in `VERSION`, kept
  within Play's version code. `RELEASE_SCOPE` gives the release commit a scope, for an app whose commits need one.
- `tooling/release.mk`, the `release` and `release-dry` targets, which `common.mk` includes and an app without it can
  include alone.

### Changed

- A release whose push is refused undoes its commit and tag, so it can be run again, and leaves any notes written up
  during it under `## Unreleased`.

## 0.6.0 - 2026-10-25

### Added

- Release checks from Callie and Jot: `greylock::ReleaseNotes`, `greylock::releases`, `greylock::PackageSystem` and
  `greylock::UpdateCheck`. They read the changelog at `:/<dir>/CHANGELOG.md`, log as `<dir>.update`, and honor
  `<env>_NO_UPDATE_CHECK`.
- `GREYLOCK_APP_REPO`, which an app now has to set, and `GREYLOCK_APP_UBUNTU_DEBS`, for an app that builds debs for
  Ubuntu apart from Debian's.

### Changed

- `greylock::core` links Qt Network.

## 0.5.0 - 2026-10-20

### Added

- `greylock::App`, the app core is built for, from `GREYLOCK_APP_ID`, `_NAME`, `_DIR`, `_ENV` and `_DEVEL`, which an
  app sets before `add_subdirectory(core)`. Core on its own is built for a made-up Demo app.
- `greylock::identity`, from Callie and Jot, which gives a development build its own id, folders and name, and tells
  when the app runs in a Flatpak (`<env>_FLATPAK` overrides it).
- `greylock::ThemeLibrary`, the user's own theme files, from Callie and Jot.

## 0.4.0 - 2026-10-20

### Added

- `greylock::core`, a static Qt 6 library an app adds with `add_subdirectory(core)` and links. It holds the theme
  engine from Callie and Jot, as `greylock::color`, `greylock::base16`, `greylock::ThemeSpec` and
  `greylock::ThemeLoader`, and compiles the built-in themes in at `:/greylock/themes/`.

### Changed

- `tooling/format.sh` works in a repository with C++ and no QML, or the other way round.

## 0.3.0 - 2026-10-20

### Added

- `tooling/common.mk`, the everyday make targets every app shares, which an app's `Makefile` includes after setting
  its own values. It adds a `logs` target that follows the app's log files. The app provides the `dev` and `ci`
  CMake presets, the `format:md` and `format:md:check` npm scripts, and a CLI with `logs --follow`, as the top of
  `common.mk` lists.

## 0.2.0 - 2026-10-19

### Added

- The built-in themes, in `themes/`, from Callie and Jot.
- `tooling/source-tarball.sh`, which writes an app's release tarball with core's themes included at `core/`.

## 0.1.0 - 2026-10-19

### Added

- The release, version check and release notes scripts, `format.sh`, the commit rules and hook, and the format and
  lint configs, from Callie and Jot. The scripts run from the app's root and read the maintainer from
  `packaging/debian/control`.
