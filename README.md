# Greylock Labs core

The code, themes and tooling that the [Greylock Labs](https://greylocklabs.org) apps share:
[Callie](https://github.com/greylocklabs/callie), [Jot](https://github.com/greylocklabs/jot) and, where it fits,
[Woodshed](https://github.com/greylocklabs/woodshed). Each app includes core as a git submodule, so a change made here
reaches an app only when the app takes a new release of core.

For now core holds the tooling, the themes and the theme engine, the start of the `greylock::core` C++ library. More of
the shared C++, the `Greylock.Ui` QML module, the command-line helpers and the shared site files move in next.

## What is here

- `CMakeLists.txt` and `cpp/` build `greylock::core`, a static Qt 6 library in the `greylock::` namespace, with the
  built-in themes compiled in at `:/greylock/themes/<id>.toml`. It needs Qt 6.8 or later, with Qt Network, and toml++.
  It holds:
  - `greylock::App` and `greylock::identity`: the app's name, id and folders.
  - The theme engine: `greylock::color` (OKLCH, contrast and hex parsing), `greylock::ThemeSpec`,
    `greylock::ThemeLoader` and `greylock::base16`.
  - `greylock::ThemeLibrary`: the user's own theme files.
  - Releases: `greylock::ReleaseNotes` reads the app's built-in changelog, `greylock::releases` and
    `greylock::PackageSystem` read GitHub's releases and pick this system's package, and `greylock::UpdateCheck` asks
    GitHub for the latest release at most once a day.

  Callie and Callie Light are the suite's built-in theme names in every app, and `callie` is the default theme every
  other inherits from.

- `themes/*.toml` are the built-in themes every app ships: Callie, Callie Light, Minimal, Minimal Dark, Paper,
  Tomorrow and Tomorrow Night. `callie.toml` sets every key, and the others inherit what they leave out.
- `tooling/release.sh`, `check-version.sh` and `release-notes.sh` release a Qt app: they date the notes in
  `CHANGELOG.md`, set the version in CMake, the RPM spec, the Debian changelog and the AppStream metainfo, run
  `make check`, then commit, tag and push. They read the app's name and id from its `CMakeLists.txt`.
- `tooling/source-tarball.sh` writes an app's release tarball with core's buildable files, such as the themes,
  included at `core/`, since `git archive` leaves submodules out.
- `tooling/common.mk` holds the everyday make targets: `build`, `run`, `logs`, `watch`, `gallery`, `screenshots`,
  `test`, `lint`, `format`, `check`, `release` and the rest. An app's Makefile sets its own values and includes it.
- `tooling/format.sh` formats or checks C++ and QML with clang-format and qmlformat.
- `tooling/commitlint.mjs` holds the commit rules; an app passes in its own scopes.
- `tooling/commit-msg` runs commitlint as a git hook.
- The format and lint configs: `.clang-format`, `.clangd`, `.qmlformat.ini`, `.qmllint.ini`, `.editorconfig`,
  `.prettierrc`, `.prettierignore`, `.markdownlint-cli2.yaml` and `cliff.toml`.

## Using it in an app

Add core on its `release` branch, which only moves when core tags a release:

```sh
git submodule add -b release https://github.com/greylocklabs/core core
```

Then, from the app's root:

- Add core to the top-level `CMakeLists.txt`, after the app's warning flags so they apply to core too, saying which
  app it is built for, and link the targets that use it to `greylock::core`:

  ```cmake
  set(GREYLOCK_APP_ID "org.example.Demo")    # the release build's id; a development build adds .Devel
  set(GREYLOCK_APP_NAME "Demo")
  set(GREYLOCK_APP_DIR "demo")               # the folder under each XDG directory, the package and the resource prefix
  set(GREYLOCK_APP_ENV "DEMO")               # the prefix of environment overrides, such as DEMO_FLATPAK
  set(GREYLOCK_APP_REPO "greylocklabs/demo") # the GitHub repository releases are published to
  set(GREYLOCK_APP_UBUNTU_DEBS OFF)          # Ubuntu takes demo-ubuntu_*.deb, built apart from Debian's demo_*.deb
  set(GREYLOCK_APP_DEVEL OFF)                # a development build, with its own identity
  add_subdirectory(core)
  ```

- Link each config into the root, where the tools look for it, for example
  `ln -s core/tooling/.clang-format .clang-format`, and the hook as
  `ln -s ../core/tooling/commit-msg .githooks/commit-msg`.
- Write `commitlint.config.mjs` as:

  ```js
  import { config } from "./core/tooling/commitlint.mjs";

  export default config(["core", "gui", "ci", "deps", "deps-dev"]);
  ```

- Write the `Makefile` as the app's own values followed by `include core/tooling/common.mk`; the top of
  `common.mk` lists the values it reads.
- Call the scripts from the root, as `core/tooling/release.sh minor`, from CI.
- Check out submodules in CI, with `submodules: true` on `actions/checkout`, and build the release tarball with
  `core/tooling/source-tarball.sh`. Add `/core export-ignore` to the app's `.gitattributes`, so the app's own
  archive leaves the empty submodule folder out; the script adds core's files back.
- Add a `gitsubmodule` entry to `.github/dependabot.yml`, so each core release opens a pull request.

`common.mk` also expects `dev` and `ci` CMake presets, with a `ci` workflow and an `all_qmllint` target, `format:md`
and `format:md:check` scripts in `package.json`, a GUI and gallery that take `--theme`, `--screenshot` and
`--live-qml`, and a CLI with `logs --follow`. The top of `common.mk` lists the same.

The scripts expect the app's files where Callie and Jot keep them: `CMakeLists.txt` with `project(<app>` and
`set(<APP>_APP_ID ...)`, `packaging/rpm/<app>.spec`, `packaging/debian/control` and `changelog`,
`data/<app id>.metainfo.xml`, and `CHANGELOG.md` with an `## Unreleased` section.

## Developing

```sh
npm install
cmake -S . -B build -G Ninja -DGREYLOCK_WERROR=ON && cmake --build build && ctest --test-dir build
tests/release_test.sh   # releases a made-up app to a local remote
tests/tarball_test.sh   # builds a made-up app's release tarball
tests/make_test.sh      # includes common.mk in a made-up app
npm run format:check
```

## Releasing core

Write the notes under `## Unreleased` in `CHANGELOG.md`, then on an up-to-date main run `scripts/release.sh 0.2.0`. It
dates the notes, tags `v0.2.0`, and moves the `release` branch to it.
