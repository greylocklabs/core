#!/usr/bin/env bash
# Runs format.sh in made-up apps against a stand-in qmlformat, which tidies "messy", fails on
# "broken", and on "unhandled" says "Unhandled state" as Qt logs it, only when told to.
#
#   tests/format_test.sh
set -euo pipefail

format="$(cd "$(dirname "$0")/../tooling" && pwd)/format.sh"
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
failures=0
fail() {
    echo "FAIL: $*"
    failures=$((failures + 1))
}

mkdir "$tmp/bin"
cat >"$tmp/bin/qmlformat" <<'QMLFORMAT'
#!/usr/bin/env bash
in_place=false
if [ "$1" = -i ]; then
    in_place=true
    shift
fi
if grep -q broken "$1"; then
    echo "$1:1:1: error: syntax error" >&2
    exit 1
fi
if grep -q unhandled "$1" && [ "${QT_FORCE_STDERR_LOGGING:-}" = 1 ]; then
    echo "Unhandled state ReturnStatement" >&2
fi
tidied="$(sed 's/messy/tidy/; s/unhandled/mangled/' "$1")"
if "$in_place"; then
    printf '%s\n' "$tidied" >"$1"
else
    printf '%s\n' "$tidied"
fi
QMLFORMAT
chmod +x "$tmp/bin/qmlformat"
export PATH="$tmp/bin:$PATH"

# A made-up app holding one QML file with `words` in it; prints its folder.
app() {
    local dir="$tmp/app-$1"
    mkdir "$dir"
    git -C "$dir" init -q
    touch "$dir/CMakeLists.txt"
    echo "Item { // $2" >"$dir/Main.qml"
    echo "$dir"
}

# Runs format.sh in `dir`, keeping what it says in $said and its exit status in $code.
run() {
    code=0
    said="$(cd "$1" && "$format" "$2" 2>&1)" || code=$?
}

dir="$(app tidy tidy)"
run "$dir" check
if [ "$code" -ne 0 ]; then
    fail "a formatted file fails check: $said"
fi

dir="$(app messy messy)"
run "$dir" check
if ! { [ "$code" -eq 1 ] && [[ "$said" == *"Main.qml is not formatted"* ]]; }; then
    fail "an unformatted file passes check: $said"
fi
run "$dir" fix
if ! { [ "$code" -eq 0 ] && grep -q tidy "$dir/Main.qml"; }; then
    fail "fix does not format: $said"
fi

dir="$(app unhandled unhandled)"
run "$dir" check
if ! { [ "$code" -eq 1 ] && [[ "$said" == *"cannot lay out Main.qml"* ]]; }; then
    fail "check passes a file qmlformat cannot lay out: $said"
fi
run "$dir" fix
if ! { [ "$code" -eq 1 ] && grep -q unhandled "$dir/Main.qml"; }; then
    fail "fix rewrites a file qmlformat cannot lay out: $said"
fi

dir="$(app broken broken)"
run "$dir" check
if ! { [ "$code" -eq 1 ] && [[ "$said" == *"syntax error"* ]] &&
    [[ "$said" == *"failed on Main.qml"* ]]; }; then
    fail "check hides a qmlformat failure: $said"
fi
run "$dir" fix
if ! { [ "$code" -eq 1 ] && [[ "$said" == *"failed on Main.qml"* ]]; }; then
    fail "fix hides a qmlformat failure: $said"
fi

if [ "$failures" -gt 0 ]; then
    echo "$failures failure(s)"
    exit 1
fi
echo "format.sh: all checks passed"
