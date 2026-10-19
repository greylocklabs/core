#!/usr/bin/env bash
# Writes the app's release tarball, <app>-<version>.tar.xz, from HEAD: the app's own files as its
# .gitattributes allows, and core's at core/, at the commit the app pins, as core's allows. git
# archive leaves submodules out, so core is archived on its own and appended.
#
#   core/tooling/source-tarball.sh 0.6.0 [output directory]
set -euo pipefail

# Run from the app's root, where make and CI run it; core sits in core/ beside the app's files.
root="$PWD"
[ -f "$root/CMakeLists.txt" ] || { echo "$(basename "$0"): run this from the app's root" >&2; exit 1; }
version="${1:?usage: source-tarball.sh VERSION [DIRECTORY]}"
out="${2:-.}"
app="$(sed -n 's/^project(\([a-z0-9-]*\).*/\1/p' CMakeLists.txt | head -1)"
# core goes in at the commit the app pins, whatever core/ has checked out.
pinned="$(git rev-parse HEAD:core)"
git -C core cat-file -e "$pinned^{commit}" 2>/dev/null ||
    { echo "source-tarball.sh: core/ lacks the pinned commit; run git submodule update --init" >&2; exit 1; }

prefix="$app-$version"
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
git archive --format=tar --prefix="$prefix/" HEAD >"$tmp/app.tar"
git -C core archive --format=tar --prefix="$prefix/core/" "$pinned" >"$tmp/core.tar"
tar --concatenate --file="$tmp/app.tar" "$tmp/core.tar"
xz -9 <"$tmp/app.tar" >"$out/$prefix.tar.xz"
echo "$out/$prefix.tar.xz"
