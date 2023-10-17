#!/bin/bash
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

BREW_PKGS="
    coreutils
"

function test_if_run_as_root() {
    MY_NAME=$(whoami)
    if [[ "${MY_NAME}" == "root" ]]; then
        cat <<-MSGEND
			Please do not run this script as a root.
			Script will ask for your password for tasks it needs
			to run as a root (sudo ...)
			MSGEND
        exit 1
    fi
}

function install_homebrew() {
    if command -v brew &> /dev/null; then
        echo "Homebrew already installed"
        return
    fi

    echo "This action is going to change your system by installing Homebrew package manager and later other packages"
    echo "Press CTRL+C if you do not want changes to your system, or press enter to continue..."
    read user_consent

    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
}

function install_brew_packages() {
    echo "Installing necessary packages..."

    BREW_PKGS=$(echo "${BREW_PKGS}" | tr "\n" " "|tr -s " ")
    brew update

    for pkg in $BREW_PKGS
    do
        brew install $pkg
    done
}

test_if_run_as_root
install_homebrew
install_brew_packages
