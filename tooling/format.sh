#!/usr/bin/env bash
# Formats or checks C++ and QML sources.
#
#   core/tooling/format.sh fix     rewrite files in place
#   core/tooling/format.sh check   exit non-zero if anything is unformatted
#
# qmlformat has no check mode, so the QML pass formats to stdout and diffs.
set -euo pipefail

mode="${1:-check}"
# Run from the app's root, where make and CI run it; core sits in core/ beside the app's files.
root="$PWD"
[ -f "$root/CMakeLists.txt" ] || { echo "$(basename "$0"): run this from the app's root" >&2; exit 1; }
cd "$root"

# qmlformat is not on PATH in most distro layouts, so ask Qt where it lives.
qmlformat_bin="$(command -v qmlformat || true)"
if [ -z "$qmlformat_bin" ]; then
    for candidate in \
        "$(qtpaths6 --query QT_INSTALL_BINS 2>/dev/null || true)/qmlformat" \
        /usr/lib64/qt6/bin/qmlformat \
        /usr/lib/qt6/bin/qmlformat \
        /usr/lib/x86_64-linux-gnu/qt6/bin/qmlformat
    do
        [ -x "$candidate" ] && { qmlformat_bin="$candidate"; break; }
    done
fi
status=0

# Command substitution propagates a git failure under set -e; process
# substitution would hide it and leave the lists empty.
cxx_list="$(git ls-files --cached --others --exclude-standard '*.cpp' '*.h')"
qml_list="$(git ls-files --cached --others --exclude-standard '*.qml')"
if [ -z "$cxx_list" ] && [ -z "$qml_list" ]; then
    echo "format: git ls-files found no sources" >&2
    exit 1
fi
# A repository may have no QML yet, as core does; an empty list must not become one empty name.
cxx=()
qml=()
[ -z "$cxx_list" ] || mapfile -t cxx <<<"$cxx_list"
[ -z "$qml_list" ] || mapfile -t qml <<<"$qml_list"

if [ "${#cxx[@]}" -eq 0 ]; then
    :
elif ! command -v clang-format >/dev/null; then
    echo "format: clang-format not found, skipping C++" >&2
    [ "$mode" = check ] && status=1
elif [ "$mode" = fix ]; then
    clang-format -i "${cxx[@]}"
else
    clang-format --dry-run -Werror "${cxx[@]}" || status=1
fi

if [ "${#qml[@]}" -eq 0 ]; then
    :
elif [ ! -x "$qmlformat_bin" ]; then
    echo "format: qmlformat not found, skipping QML" >&2
    [ "$mode" = check ] && status=1
elif [ "$mode" = fix ]; then
    "$qmlformat_bin" -i "${qml[@]}"
elif ! command -v diff >/dev/null; then
    echo "format: diff not found, cannot check QML" >&2
    status=1
else
    for f in "${qml[@]}"; do
        if ! "$qmlformat_bin" "$f" | diff -q - "$f" >/dev/null; then
            echo "format: $f is not formatted" >&2
            status=1
        fi
    done
fi

exit "$status"
