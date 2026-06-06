#!/usr/bin/env bash

set -u

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SUITE_FILE="${ROOT_DIR}/perftsuite.epd"
ENGINE_BIN="${ROOT_DIR}/perft_cli"
REPORT_FILE="${ROOT_DIR}/perftsuite_discrepancies.txt"
MAX_CASES=0
MAX_DEPTH=0

while [[ $# -gt 0 ]]; do
    case "$1" in
    --max-cases)
        MAX_CASES="$2"
        shift 2
        ;;
    --max-depth)
        MAX_DEPTH="$2"
        shift 2
        ;;
    *)
        echo "Unknown argument: $1" >&2
        echo "Usage: $0 [--max-cases N] [--max-depth N]" >&2
        exit 1
        ;;
    esac
done

if [[ ! -f "${SUITE_FILE}" ]]; then
    echo "Could not find perftsuite file at ${SUITE_FILE}" >&2
    exit 1
fi

echo "Building perft_cli..."
clang++ -std=c++20 -O2 -Wall -Wextra -Wpedantic \
    "${ROOT_DIR}/perft_cli.cpp" \
    "${ROOT_DIR}/init.cpp" \
    "${ROOT_DIR}/bitboards.cpp" \
    "${ROOT_DIR}/hashkeys.cpp" \
    "${ROOT_DIR}/board.cpp" \
    "${ROOT_DIR}/data.cpp" \
    "${ROOT_DIR}/attack.cpp" \
    "${ROOT_DIR}/io.cpp" \
    "${ROOT_DIR}/movegen.cpp" \
    "${ROOT_DIR}/validate.cpp" \
    "${ROOT_DIR}/makemove.cpp" \
    "${ROOT_DIR}/perft.cpp" \
    -o "${ENGINE_BIN}"

if [[ $? -ne 0 ]]; then
    echo "Build failed" >&2
    exit 1
fi

start_epoch=$(date +%s)

case_count=0
total_checks=0
pass_count=0
fail_count=0
error_count=0

{
    echo "Perft Suite Discrepancy Report"
    echo "Generated: $(date)"
    echo "Suite: ${SUITE_FILE}"
    echo
} > "${REPORT_FILE}"

while IFS= read -r raw_line || [[ -n "${raw_line}" ]]; do
    line="${raw_line%$'\r'}"

    if [[ -z "${line//[[:space:]]/}" ]]; then
        continue
    fi

    if [[ "${line:0:1}" == "#" ]]; then
        continue
    fi

    IFS=';' read -r -a fields <<< "${line}"
    fen="$(echo "${fields[0]}" | sed -E 's/[[:space:]]+$//')"

    if [[ -z "${fen}" ]]; then
        continue
    fi

    if [[ ${MAX_CASES} -gt 0 && ${case_count} -ge ${MAX_CASES} ]]; then
        break
    fi

    ((case_count++))

    for ((i = 1; i < ${#fields[@]}; i++)); do
        token="$(echo "${fields[i]}" | sed -E 's/^[[:space:]]+//; s/[[:space:]]+$//')"

        if [[ -z "${token}" ]]; then
            continue
        fi

        if [[ "${token}" =~ ^D([0-9]+)[[:space:]]+([0-9]+)$ ]]; then
            depth="${BASH_REMATCH[1]}"
            expected="${BASH_REMATCH[2]}"

            if [[ ${MAX_DEPTH} -gt 0 && ${depth} -gt ${MAX_DEPTH} ]]; then
                continue
            fi

            ((total_checks++))

            actual_output="$(${ENGINE_BIN} "${fen}" "${depth}" 2>&1)"
            cmd_status=$?

            if [[ ${cmd_status} -ne 0 ]]; then
                ((error_count++))
                {
                    echo "Case ${case_count}, depth D${depth}: execution error"
                    echo "FEN: ${fen}"
                    echo "Expected: ${expected}"
                    echo "Error: ${actual_output}"
                    echo
                } >> "${REPORT_FILE}"
                continue
            fi

            if [[ ! "${actual_output}" =~ ^[0-9]+$ ]]; then
                ((error_count++))
                {
                    echo "Case ${case_count}, depth D${depth}: invalid output"
                    echo "FEN: ${fen}"
                    echo "Expected: ${expected}"
                    echo "Actual raw output: ${actual_output}"
                    echo
                } >> "${REPORT_FILE}"
                continue
            fi

            actual="${actual_output}"

            if [[ "${actual}" == "${expected}" ]]; then
                ((pass_count++))
            else
                ((fail_count++))
                {
                    echo "Case ${case_count}, depth D${depth}: mismatch"
                    echo "FEN: ${fen}"
                    echo "Expected: ${expected}"
                    echo "Actual:   ${actual}"
                    echo
                } >> "${REPORT_FILE}"
            fi
        fi
    done

done < "${SUITE_FILE}"

end_epoch=$(date +%s)
elapsed=$((end_epoch - start_epoch))

{
    echo "Summary"
    echo "Cases processed: ${case_count}"
    echo "Checks run:      ${total_checks}"
    echo "Passed:          ${pass_count}"
    echo "Mismatches:      ${fail_count}"
    echo "Errors:          ${error_count}"
    echo "Elapsed seconds: ${elapsed}"
} >> "${REPORT_FILE}"

echo "Perft suite run complete."
echo "Report written to ${REPORT_FILE}"
echo "Cases: ${case_count}, checks: ${total_checks}, passed: ${pass_count}, mismatches: ${fail_count}, errors: ${error_count}, elapsed: ${elapsed}s"
