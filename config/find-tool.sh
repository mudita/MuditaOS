#!/usr/bin/env bash

# function which checks if minimum clang tooling requirements are set
clang_tools_assert() {
    # check if clang-format in path is in proper version, version is 3rd column in `clang-format --version`
    CVER=$( [[ $(which clang-format) ]] && echo $(clang-format --version | cut -d ' ' -f 3 | cut -d '.' -f 1) || echo "0")
    # check for either clang-format or clang-format-9
    if [[ $CVER -lt 10 && ! $(which clang-format-10) ]]; then
        cat << EOF > 1
Either install:
    clang-format in at least version 10 and set as default"
    or
    clang-format-10

    Your clang format version in path used:
        $(clang-format --version):
        $(clang-format-10 --version)
    git commit aborted"
EOF
        exit 1
    fi
}

# function to search clang tooling in paths common for debian, ubuntu and archlinux
get_clang_script() {
    local script_to_find="$1"
    local declare searchpaths=(
        $(which "$script_to_find" 2>&1>/dev/null)                  # clang-format-diff in path
        "/usr/share/clang/clang-format-10/$script_to_find"         # clang-format-diff location for clang format 9 Ubuntu/Debian 18.04
        "/usr/share/clang/clang-format-${CVER}/$script_to_find"    # clang-format-diff location on Ubuntu/Debian
        "/usr/share/clang/$script_to_find"                         # clang-format_diff location on Arch last resort
    )
    local tool=""
    for el in ${searchpaths[@]}; do
        if [[ -f ${el} ]]; then
            tool=${el}
            break
        fi
    done
    if [[ ${tool} == "" ]]; then
        echo $script_to_find not found in path and: ${searchpaths[@]} >2
    fi
    echo "${tool}"
}

# function to search for clang-format python script for formatting
get_clang_format_script() {
    get_clang_script clang-format-diff.py
}

# function to search for clang-tidy python script for errors
get_clang_tidy_script()
{
    get_clang_script clang-tidy-diff.py
}
