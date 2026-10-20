# Changelog

What changed in each release of core, for the people working on the apps that use it. Notes collect under Unreleased,
and `scripts/release.sh` dates them.

## Unreleased

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
