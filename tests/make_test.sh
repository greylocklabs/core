#!/usr/bin/env bash
# Includes core/tooling/common.mk from a made-up app's Makefile, and release.mk alone from another's,
# and checks, with make -n, that the targets pick up the app's own values. Nothing is built.
#
#   tests/make_test.sh
set -euo pipefail

tooling="$(cd "$(dirname "$0")/../tooling" && pwd)"
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
failures=0
fail() {
    echo "FAIL: $*"
    failures=$((failures + 1))
}

mkdir -p "$tmp/core/themes"
ln -s "$tooling" "$tmp/core/tooling"
touch "$tmp/core/themes/demo.toml" "$tmp/core/themes/demo-light.toml" "$tmp/CMakePresets.json"
cd "$tmp"
cat >Makefile <<'MAKE'
GUI_TARGET := demo-gui
CLI_TARGET := demo
GALLERY_TARGET := demo-gallery
LOG_CATEGORIES := demo.*
SHOT_APP := --sample
FEDORA_DEPS := demo-fedora-deps
DEBIAN_DEPS := demo-debian-deps
include core/tooling/common.mk
MAKE
mkdir -p build/dev && touch build/dev/.preset-applied

expect() {
    local target="$1" want="$2" out
    out="$(make -n "$target" 2>&1)" || fail "make -n $target failed: $out"
    [[ $out == *"$want"* ]] || fail "make -n $target lacks \"$want\":
$out"
}

help="$(make help)"
for target in deps setup configure build run logs cli gallery watch watch-gallery screenshots test lint \
    format check clean release release-dry; do
    [[ $help == *"make $target "* ]] || fail "make help does not list $target"
done
expect run "build/dev/src/gui/demo-gui"
expect run 'QT_LOGGING_RULES="qml.debug=true;demo.*=true"'
expect logs "build/dev/src/cli/demo logs --follow"
expect cli "build/dev/src/cli/demo"
expect gallery "build/dev/src/gallery/demo-gallery"
expect screenshots "--sample --theme"
expect screenshots "app-\$theme.png"
expect deps "demo-fedora-deps"
expect release "core/tooling/release.sh"
[ "$(make -n release minor 2>&1 | grep -c 'release.sh minor')" -ge 1 ] || fail "make release minor does not pass the level"

# An app without common.mk, as an Android one, takes the release targets alone, with its commit scope.
mkdir -p "$tmp/android/core"
ln -s "$tooling" "$tmp/android/core/tooling"
printf 'RELEASE_SCOPE := release\ninclude core/tooling/release.mk\n' >"$tmp/android/Makefile"
out="$(make -C "$tmp/android" -n release-dry patch 2>&1)" || fail "make -n release-dry failed alone: $out"
[[ $out == *"RELEASE_SCOPE=release core/tooling/release.sh patch --dry-run"* ]] ||
    fail "release.mk alone does not pass the scope and level: $out"

if [ "$failures" -gt 0 ]; then
    echo "$failures check(s) failed"
    exit 1
fi
echo "all make checks passed"
