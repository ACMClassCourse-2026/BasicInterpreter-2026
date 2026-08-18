#!/bin/sh

set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cases_dir="$script_dir/tests/cases"
build_dir="$script_dir/build/run_tests"
executable="$build_dir/code"
total_cases=400
passed_cases=0
failed_cases=0

if ! command -v cmake >/dev/null 2>&1; then
    echo "cmake not found" >&2
    exit 2
fi

if ! command -v make >/dev/null 2>&1; then
    echo "make not found" >&2
    exit 2
fi

if [ ! -d "$cases_dir" ]; then
    echo "test directory not found: $cases_dir" >&2
    exit 2
fi

echo "Configuring build with CMake..."
if ! cmake \
    -S "$script_dir" \
    -B "$build_dir" \
    -G "Unix Makefiles" \
    -DBUILD_TESTING=OFF; then
    echo "CMake configuration failed." >&2
    exit 1
fi

echo "Building interpreter with make..."
if ! make -C "$build_dir" code; then
    echo "Make build failed." >&2
    exit 1
fi

echo "Running $total_cases test cases..."
case_number=0
while [ "$case_number" -lt "$total_cases" ]; do
    case_id=$(printf 'case_%03d' "$case_number")
    input_path="$cases_dir/$case_id.in"
    expected_path="$cases_dir/$case_id.out"
    actual_path="$build_dir/$case_id.actual"
    stderr_path="$build_dir/$case_id.stderr"

    if [ ! -f "$input_path" ] || [ ! -f "$expected_path" ]; then
        echo "FAIL $case_id: input or expected output is missing"
        failed_cases=$((failed_cases + 1))
        case_number=$((case_number + 1))
        continue
    fi

    if "$executable" <"$input_path" >"$actual_path" 2>"$stderr_path"; then
        exit_status=0
    else
        exit_status=$?
    fi

    if [ "$exit_status" -ne 0 ]; then
        echo "FAIL $case_id: interpreter exited with status $exit_status"
        if [ -s "$stderr_path" ]; then
            sed 's/^/  stderr: /' "$stderr_path"
        fi
        failed_cases=$((failed_cases + 1))
    elif cmp -s "$expected_path" "$actual_path"; then
        echo "PASS $case_id"
        passed_cases=$((passed_cases + 1))
    else
        echo "FAIL $case_id: output mismatch"
        diff -u "$expected_path" "$actual_path" || true
        failed_cases=$((failed_cases + 1))
    fi

    case_number=$((case_number + 1))
done

echo "Passed: $passed_cases/$total_cases"
echo "Failed: $failed_cases/$total_cases"

if [ "$failed_cases" -eq 0 ] && [ "$passed_cases" -eq "$total_cases" ]; then
    exit 0
fi
exit 1
