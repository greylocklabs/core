# Repo notes - core

Facts the review bot cannot infer from one pass over the diff. The philosophy, the finding categories, the workflow,
the scoring and the output format all live in the bot's base prompt; this file only refines them for this repo.

## What this repo is

Shared tooling, and later shared code, for the Greylock Labs apps: Callie (calendar) and Jot (notes), both Qt 6 and
C++20 on Linux, and Woodshed (Android). Each app includes this repo as a git submodule at `core/`, tracking the
`release` branch, which moves only when core tags a release.

| Path                     | What it is                                                                       |
| ------------------------ | -------------------------------------------------------------------------------- |
| `tooling/*.sh`           | Bash scripts the apps run from their own root, as `core/tooling/<script>`.       |
| `tooling/commitlint.mjs` | The commit rules; each app calls `config(scopes)`.                               |
| `tooling/` dotfiles      | Format and lint configs, symlinked into each app's root.                         |
| `scripts/release.sh`     | Releases core itself and moves the `release` branch.                             |
| `tests/release_test.sh`  | Releases a made-up app to a local bare remote, to exercise `tooling/release.sh`. |

## What to check

- **App neutrality.** Nothing in `tooling/` may name Callie, Jot or Woodshed, or assume one app's files. A script
  reads what it needs from the app's own files (`CMakeLists.txt`, `packaging/`, `data/`, `CHANGELOG.md`).
- **Working directory.** The scripts run from the app's root, never from `core/`. Paths to the app's files are
  relative to that root; paths to sibling scripts go through the script's own directory.
- **Breaking the apps.** A change here reaches every app at its next pin update. Renaming a script, changing its
  arguments or output, or tightening a config is a breaking change and needs a changelog entry saying what an app
  must change.
- **Release safety.** `tooling/release.sh` commits, tags and pushes. Any change to it should keep the guarantees that
  `tests/release_test.sh` checks: a dry run changes nothing, a failed `make check` leaves the tree and notes as they
  were, and a release reaches origin with its tag in one atomic push. A change to those paths without a matching test
  change is a finding.
