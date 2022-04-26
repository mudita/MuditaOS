#!/bin/bash
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

set -euo pipefail
#set -o posix
LC_CTYPE="C"


if [ -z "${L_GIT_DIR+unbound}" ] && [ -z "$(global L_GIT_DIR="$(git rev-parse --show-toplevel 2>/dev/null)")" ]; then
  global L_GIT_DIR="$(readlink -f "$(dirname "$0")/../..")"
fi

. "${L_GIT_DIR}/tools/version_check.sh"

_extract_number () { echo "${1}" | sed -e "s|[^[:digit:]]*\([[:digit:]]\+\)|\1|"; }
_do_search () { find "${2}" -name "${1}-[[:digit:]][[:digit:]]*"; }

_get_latest () {
  latest=${1}
  set -f; IFS=":";
  for p in $PATH; do
    for file in $(_do_search "${1}" "${p}"); do
      if [ "$(expr "$(_extract_number "${latest}")" ">" "$(_extract_number "${file}")")" ]; then
        latest="${file}"
      fi
    done
  done
  echo "${latest}"
}

verify_clang_format_version()
{
  latest=$(_get_latest "clang-format")
  if less_than_version "$(${latest} --version)" "11.0.0"
  then
        cat << EOF >&1
Either install:
    clang-format in at least version 11 and set as default"
    or
    clang-format-11

    Your clang format version in path used:
        $(clang-format --version):
        $(clang-format-11 --version)
    git commit aborted"
EOF
        exit 1
    fi
}

## locate clang-format-diff
get_clang_format() {
  command -v "$(_get_latest "clang-format-diff")"
}

## locate clang-tidy-diff
get_clang_tidy() {
  command -v "$(_get_latest "clang-tidy-diff")"
}
