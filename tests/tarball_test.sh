#!/usr/bin/env bash
# Builds a made-up app's release tarball with core/tooling/source-tarball.sh and checks what goes in:
# the app's own files, even under a src/core/ of its own, and core's themes, but none of core's
# tooling and none of what the app's .gitattributes leaves out.
#
#   tests/tarball_test.sh
set -euo pipefail

repo="$(cd "$(dirname "$0")/.." && pwd)"
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
failures=0
fail() {
    echo "FAIL: $*"
    failures=$((failures + 1))
}
export GIT_AUTHOR_NAME=Demo GIT_AUTHOR_EMAIL=hello@example.org
export GIT_COMMITTER_NAME=Demo GIT_COMMITTER_EMAIL=hello@example.org

# A core like this one, with this repository's .gitattributes.
core="$tmp/core"
mkdir -p "$core/themes" "$core/tooling"
cp "$repo/.gitattributes" "$repo/LICENSE" "$core/"
echo 'name = "Demo"' >"$core/themes/demo.toml"
echo 'echo hi' >"$core/tooling/release.sh"
git -C "$core" init -q -b main
git -C "$core" add -A
git -C "$core" commit -q -m "chore: start"

app="$tmp/demo"
mkdir -p "$app/src/core"
cd "$app"
printf 'project(demo\n    VERSION 1.0.0\n)\n' >CMakeLists.txt
echo 'int main() {}' >src/core/main.cpp
printf '/core export-ignore\n/.gitmodules export-ignore\nnotes.md export-ignore\n' >.gitattributes
echo "draft" >notes.md
git init -q -b main
git -c protocol.file.allow=always submodule add -q "$core" core
git add -A
git commit -q -m "chore: start"

tarball="$("$repo/tooling/source-tarball.sh" 1.0.0 "$tmp")" || fail "source-tarball.sh failed"
[ "$tarball" = "$tmp/demo-1.0.0.tar.xz" ] || fail "the tarball is at $tarball"
files="$(tar -tJf "$tarball" | sort)"
for want in demo-1.0.0/CMakeLists.txt demo-1.0.0/src/core/main.cpp demo-1.0.0/core/themes/demo.toml \
    demo-1.0.0/core/LICENSE; do
    grep -qx "$want" <<<"$files" || fail "$want is missing"
done
for unwanted in demo-1.0.0/notes.md demo-1.0.0/.gitmodules demo-1.0.0/core/tooling/release.sh \
    demo-1.0.0/core/.gitattributes; do
    ! grep -qx "$unwanted" <<<"$files" || fail "$unwanted should be left out"
done

rm -rf core/themes
if "$repo/tooling/source-tarball.sh" 1.0.0 "$tmp" >/dev/null 2>&1; then
    fail "source-tarball.sh made a tarball without core checked out"
fi

if [ "$failures" -gt 0 ]; then
    echo "$failures check(s) failed"
    exit 1
fi
echo "all tarball checks passed"
