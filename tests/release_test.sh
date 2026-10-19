#!/usr/bin/env bash
# Releases a made-up app with core/tooling/release.sh: a dry run, a release whose make check fails,
# then one that goes out. The app's origin is a bare repository in a temporary folder, so nothing
# leaves the machine.
#
#   tests/release_test.sh
set -euo pipefail

tooling="$(cd "$(dirname "$0")/../tooling" && pwd)"
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
failures=0
fail() {
    echo "FAIL: $*"
    failures=$((failures + 1))
}

app="$tmp/demo"
mkdir -p "$app/core" "$app/packaging/rpm" "$app/packaging/debian" "$app/data"
ln -s "$tooling" "$app/core/tooling"
cd "$app"
cat >CMakeLists.txt <<'EOF'
cmake_minimum_required(VERSION 3.28)

project(demo
    VERSION 0.1.0
    LANGUAGES CXX
)
set(DEMO_APP_ID "org.example.Demo" CACHE STRING "Reverse-DNS application id")
EOF
cat >packaging/rpm/demo.spec <<'EOF'
Name:           demo
Version:        0.1.0
Release:        1%{?dist}

%changelog
* Mon Oct 12 2026 Demo Maintainer <hello@example.org> - 0.1.0-1
- New upstream release.
EOF
cat >packaging/debian/control <<'EOF'
Source: demo
Maintainer: Demo Maintainer <hello@example.org>
EOF
cat >packaging/debian/changelog <<'EOF'
demo (0.1.0-1) unstable; urgency=medium

  * New upstream release.

 -- Demo Maintainer <hello@example.org>  Mon, 12 Oct 2026 10:00:00 -0400
EOF
cat >data/org.example.Demo.metainfo.xml <<'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<component type="desktop-application">
  <id>org.example.Demo</id>
  <releases>
    <release version="0.1.0" date="2026-10-12">
      <description>
        <ul>
          <li>The first release.</li>
        </ul>
      </description>
    </release>
  </releases>
</component>
EOF
cat >CHANGELOG.md <<'EOF'
# Changelog

## Unreleased

### Added

- A second feature.

## 0.1.0 - 2026-10-12

### Added

- The first release.
EOF
# make check passes only when CHECK_OK=1.
# shellcheck disable=SC2016 # the $$ is for make
printf 'check:\n\t@test "$${CHECK_OK:-0}" = 1\n' >Makefile

git init -q -b main
git -c user.name=Demo -c user.email=hello@example.org commit -q --allow-empty -m "chore: start"
git add -A
git -c user.name=Demo -c user.email=hello@example.org commit -q -m "feat: a second feature"
git init -q --bare "$tmp/origin.git"
git remote add origin "$tmp/origin.git"
git push -q origin main
git fetch -q origin
export GIT_AUTHOR_NAME=Demo GIT_AUTHOR_EMAIL=hello@example.org
export GIT_COMMITTER_NAME=Demo GIT_COMMITTER_EMAIL=hello@example.org

core/tooling/check-version.sh || fail "check-version.sh rejects the starting versions"
sed -i 's/^## 0.1.0 - 2026-10-12$/## 0.1.0/' CHANGELOG.md
if core/tooling/check-version.sh 2>/dev/null; then
    fail "check-version.sh accepts a released app whose CHANGELOG.md lost its dated heading"
fi
git checkout -q -- CHANGELOG.md
[ "$(core/tooling/release-notes.sh 0.1.0 | sed "/./,\$!d")" = "### Added

- The first release." ] || fail "release-notes.sh prints the wrong 0.1.0 notes"

out="$(core/tooling/release.sh minor --dry-run 2>&1)" || fail "the dry run failed: $out"
[[ $out == *"Demo 0.1.0 to 0.2.0"* ]] || fail "the dry run does not name the release: $out"
[ -z "$(git status --porcelain)" ] || fail "the dry run changed the tree"

if out="$(RELEASE_YES=1 core/tooling/release.sh minor 2>&1)"; then
    fail "a release went out although make check failed"
fi
[[ $out == *"make check failed on 0.2.0"* ]] || fail "a failed make check is not explained: $out"
[ -z "$(git status --porcelain)" ] || fail "a failed make check left changes: $(git status --porcelain)"
grep -qx "## Unreleased" CHANGELOG.md || fail "a failed make check did not put back ## Unreleased"
[ -z "$(git tag -l v0.2.0)" ] || fail "a failed make check left a tag"

CHECK_OK=1 RELEASE_YES=1 core/tooling/release.sh minor >/dev/null 2>&1 || fail "the release failed"
[ "$(git log -1 --format=%s)" = "chore: release 0.2.0" ] || fail "the release commit is missing"
[ "$(git -C "$tmp/origin.git" rev-parse "v0.2.0^{commit}" 2>/dev/null)" = "$(git rev-parse HEAD)" ] ||
    fail "the tag did not reach origin"
core/tooling/check-version.sh 0.2.0 || fail "the release left the versions disagreeing"
grep -q "^\* .* Demo Maintainer <hello@example.org> - 0.2.0-1$" packaging/rpm/demo.spec ||
    fail "the RPM changelog does not name the control file's maintainer"
grep -q '<release version="0.2.0"' data/org.example.Demo.metainfo.xml || fail "the metainfo has no 0.2.0"

if [ "$failures" -gt 0 ]; then
    echo "$failures check(s) failed"
    exit 1
fi
echo "all release checks passed"
