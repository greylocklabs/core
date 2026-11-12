#!/bin/sh
# Fails when the code has strings a translation file does not, or has lost some it has, so the
# files people edit always match the app. `make translations` brings them up to date.
#
#   translations_test.sh lupdate cmake GenerateLUpdateProject.cmake project.cmake file.ts...
set -u

lupdate="$1"
cmake="$2"
generate="$3"
project_in="$4"
shift 4
work="$(mktemp -d)"
trap 'rm -rf "$work"' EXIT
# The project lupdate reads, made as the build's own update does.
"$cmake" -DIN_FILE="$project_in" -DOUT_FILE="$work/project.json" -P "$generate" ||
    { echo "could not make lupdate's project" >&2; exit 1; }
status=0
for words in "$@"; do
    copy="$work/$(basename "$words")"
    cp "$words" "$copy"
    "$lupdate" -silent -project "$work/project.json" -ts "$copy" -locations none -no-obsolete \
        >/dev/null 2>&1 || { echo "lupdate failed on $words" >&2; exit 1; }
    # cmake compares the files, as diff is not on every system that builds the apps.
    if ! "$cmake" -E compare_files "$words" "$copy"; then
        echo "$words is out of date with the code; run make translations" >&2
        status=1
    fi
done
[ "$status" -eq 0 ] && echo "the translation files match the code"
exit "$status"
