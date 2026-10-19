#!/usr/bin/env bash
# Releases core itself: dates the ## Unreleased notes in CHANGELOG.md, commits, tags vX.Y.Z, and
# moves the release branch to the tag. The apps' submodules track that branch, so Dependabot then
# opens a pull request in each app to take the release.
#
#   scripts/release.sh 0.2.0 [--dry-run]
set -euo pipefail
cd "$(dirname "$0")/.."

fail() {
    echo "release: $*" >&2
    exit 1
}

next="${1:-}"
[[ $next =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]] || fail "usage: scripts/release.sh X.Y.Z [--dry-run]"
dry_run=0
[ "${2:-}" != --dry-run ] || dry_run=1

[ "$(git rev-parse --abbrev-ref HEAD)" = main ] || fail "must be on main"
[ -z "$(git status --porcelain --untracked-files=no)" ] || fail "tracked changes are not committed"
git fetch --quiet origin
[ "$(git rev-parse HEAD)" = "$(git rev-parse origin/main)" ] || fail "main is not the same as origin/main"
! git rev-parse --quiet --verify "refs/tags/v$next" >/dev/null || fail "v$next is already tagged"
grep -qx "## Unreleased" CHANGELOG.md || fail "CHANGELOG.md has no ## Unreleased notes"

today="$(date +%F)"
if [ "$dry_run" = 1 ]; then
    echo "Would date ## Unreleased as ## $next - $today, commit \"chore: release $next\", tag v$next,"
    echo "push both, and move the release branch to v$next."
    exit 0
fi
sed -i "s/^## Unreleased\$/## $next - $today/" CHANGELOG.md
git add CHANGELOG.md
git commit --quiet -m "chore: release $next"
git tag -a "v$next" -m "core $next"
git push --atomic --follow-tags origin main
git push origin "v$next^{commit}:refs/heads/release"
echo "Released v$next; the release branch is there too."
