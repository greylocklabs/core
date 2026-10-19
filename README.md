# Greylock Labs core

The code, themes and tooling that the [Greylock Labs](https://greylocklabs.org) apps share:
[Callie](https://github.com/greylocklabs/callie), [Jot](https://github.com/greylocklabs/jot) and, where it fits,
[Woodshed](https://github.com/greylocklabs/woodshed). Each app includes core as a git submodule, so a change made here
reaches an app only when the app takes a new release of core.

For now core holds the tooling and the themes. The `greylock::` C++ library, the `Greylock.Ui` QML module, the
command-line helpers and the shared site files move in next, in that order.

## What is here

- `themes/*.toml` are the built-in themes every app ships: Callie, Callie Light, Minimal, Minimal Dark, Paper,
  Tomorrow and Tomorrow Night. `callie.toml` sets every key, and the others inherit what they leave out.
- `tooling/release.sh`, `check-version.sh` and `release-notes.sh` release a Qt app: they date the notes in
  `CHANGELOG.md`, set the version in CMake, the RPM spec, the Debian changelog and the AppStream metainfo, run
  `make check`, then commit, tag and push. They read the app's name and id from its `CMakeLists.txt`.
- `tooling/source-tarball.sh` writes an app's release tarball with core's buildable files, such as the themes,
  included at `core/`, since `git archive` leaves submodules out.
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

- Link each config into the root, where the tools look for it, for example
  `ln -s core/tooling/.clang-format .clang-format`, and the hook as
  `ln -s ../core/tooling/commit-msg .githooks/commit-msg`.
- Write `commitlint.config.mjs` as:

  ```js
  import { config } from "./core/tooling/commitlint.mjs";

  export default config(["core", "gui", "ci", "deps", "deps-dev"]);
  ```

- Call the scripts from the root, as `core/tooling/release.sh minor`, from the Makefile and CI.
- Check out submodules in CI, with `submodules: true` on `actions/checkout`, and build the release tarball with
  `core/tooling/source-tarball.sh`. Add `/core export-ignore` to the app's `.gitattributes`, so the app's own
  archive leaves the empty submodule folder out; the script adds core's files back.
- Add a `gitsubmodule` entry to `.github/dependabot.yml`, so each core release opens a pull request.

The scripts expect the app's files where Callie and Jot keep them: `CMakeLists.txt` with `project(<app>` and
`set(<APP>_APP_ID ...)`, `packaging/rpm/<app>.spec`, `packaging/debian/control` and `changelog`,
`data/<app id>.metainfo.xml`, and `CHANGELOG.md` with an `## Unreleased` section.

## Developing

```sh
npm install
tests/release_test.sh   # releases a made-up app to a local remote
tests/tarball_test.sh   # builds a made-up app's release tarball
npm run format:check
```

## Releasing core

Write the notes under `## Unreleased` in `CHANGELOG.md`, then on an up-to-date main run `scripts/release.sh 0.2.0`. It
dates the notes, tags `v0.2.0`, and moves the `release` branch to it.
