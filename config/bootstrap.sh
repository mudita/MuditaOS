#!/bin/bash -e
#####################################################################
#
#                   !!! WARNING !!!
#
# this script was written for `raw` ubuntu 18.10 64b
# optionaly consider installing ccache:
#     `sudo apt install ccache`
# it's support is added to default ccache

# packages settings
SCRIPT=$(readlink -f $0)
SCRIPT_DIR="$(dirname ${SCRIPT})"

. ${SCRIPT_DIR}/bootstrap_config

function printVar(){
    echo "$1: '${!1}'"
}

function add_to_path() {
    echo -e "\e[32m${FUNCNAME[0]} $1 $2\e[0m"
    TOOL_NAME=$1
    TOOL_DIR=$2
    BASH_RC="${HOME}/.bashrc"
    TOOL_PATH_VAR="${TOOL_NAME}_PATH"
    set +e
    check_if_exists="`cat ${BASH_RC} | grep \"${TOOL_PATH_VAR}\"`"
    set -e

    if [[ -n ${check_if_exists} ]]; then
        SED_SCRIPT="s%${TOOL_PATH_VAR}=\".*\"%${TOOL_PATH_VAR}=\"${TOOL_DIR}\"%"
        sed -e "$SED_SCRIPT" -i ${BASH_RC}
    else
        echo "export ${TOOL_PATH_VAR}=\"${TOOL_DIR}\"" >> ${BASH_RC}
        echo "PATH=\"\${${TOOL_PATH_VAR}:+\${${TOOL_PATH_VAR}}:}\${PATH}\"" >> ${BASH_RC}
    fi
}
function install_hooks(){
    echo -e "\e[32m${FUNCNAME[0]}\e[0m"
    cat <<-MSGEND
		Install style checking hooks
		by default hook is reportin error only
		if you would like to make it automatically fix style errors add config "user.fixinstage" to your git configuration:
		    git config user.fixinstage true
		MSGEND
    HOOK="pre-commit.hook"

    L_GIT_DIR=$(git rev-parse --show-toplevel)
    ln -sf ${L_GIT_DIR}/config/${HOOK} ${L_GIT_DIR}/.git/hooks/pre-commit
}

function add_ignore_revs_for_blame() {
    echo -e "\e[32m${FUNCNAME[0]}\e[0m"
    git config blame.ignoreRevsFile .gitblameignorerevs
}

function setup_gcc_alternatives() {
    echo -e "\e[32m${FUNCNAME[0]}\e[0m"
    cat <<-MSGEND
		# set gcc-9 as default alternative (instead of default current in ubuntu)
		sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 900 --slave /usr/bin/g++ g++ /usr/bin/g++-9
		MSGEND
}

function install_ubuntu_packages() {
    echo -e "\e[32m${FUNCNAME[0]}\e[0m"
    cat <<-MSGEND
		# Install neccessary packages more or less this should be complete list:
		binutils wget git make pkg-config gtkmm-3.0 gcc-9 g++-9 portaudio19-dev
		# for ubutnut 18.04 you can
		sudo add-apt-repository ppa:ubuntu-toolchain-r/test
		sudo apt update
		sudo apt install gcc-9 g++-9
		# 
		# don't forget to update alternatives:
		MSGEND
    setup_gcc_alternatives
}

function setup_arm_toolchain() {
    echo -e "\e[32m${FUNCNAME[0]}\e[0m"
    pushd /tmp
    echo "Download armgcc"
    [[ ! -f $ARM_GCC ]] &&
        wget --no-verbose --show-progress -O ${ARM_GCC_PACKAGE_FILE} ${ARM_GCC_SOURCE_LINK}
    # untar to HOME
    if [[ ! -d ${HOME}/${ARM_GCC} ]]; then
        echo "extracting: ${ARM_GCC_PACKAGE_FILE} to ${HOME}"
        tar -xjf ${ARM_GCC_PACKAGE_FILE} -C ${HOME}/
    fi
    echo "ARM GCC installed to ${HOME}/${ARM_GCC}"
    popd 
}

function setup_cmake() {
    echo -e "\e[32m${FUNCNAME[0]}\e[0m"
    pushd /tmp
    [[ ! -f ${CMAKE_DIR}.tgz ]] &&
        wget --no-verbose -O ${CMAKE_PKG} ${CMAKE_SOURCE_LINK}
    [[ ! -d ${HOME}/${CMAKE_DIR} ]] &&
        tar -xf ${CMAKE_PKG} -C ${HOME}/
    popd
    echo "CMAKEV installed to ${HOME}/${CMAKE_DIR} and set in PATH"
}

function install_docker() {
    if [[ -n $(command -v docker) ]]; then
        TESTED_VERSION="19.03.8"
        DOCKER_VERSION=$(docker version -f '{{.Server.Version}}')
        if [[ ${TESTED_VERSION} != ${DOCKER_VERSION} ]]; then
            echo -e "Tested with docker ${DOCKER_VERSION} your is ${DOCKER_VERSION} consider updating"
        else
            echo "Docker already installed"
        fi

    else
        curl -fsSL https://get.docker.com -o /tmp/get-docker.sh
        sudo sh /tmp/get-docker.sh
    fi
}


BUILD_STEPS=(
        install_hooks
        add_ignore_revs_for_blame
        install_ubuntu_packages
        setup_arm_toolchain
        setup_cmake
        setup_gcc_alternatives
        "add_to_path ${ARM_GCC_PATH_VAR} ${HOME}/${ARM_GCC}/bin"
        "add_to_path ${CMAKE_PATH_VAR} ${HOME}/${CMAKE_DIR}/bin"
        install_docker
        )



if [ $# -eq 1 ]; then 
    PARAM=$1
    if [ "${PARAM:$(( ${#PARAM} - 1 )):1}" == "-" ]; then
        START=${PARAM%*-}
        END=${#BUILD_STEPS[@]}
    else
        START=${PARAM}
        END=$(( ${PARAM} + 1 ))
    fi
else
    echo "Available build steps:"
    I=0
    while [ $I -lt ${#BUILD_STEPS[@]} ]
    do
        echo -e "\t$I ${BUILD_STEPS[${I}]}"
        I=$(( $I + 1 ))
    done
    cat <<- MSGEND
		call:
			${0} <build step no>[-]
		ex.:
			${0} 1       - build step 1 (${BUILD_STEPS[1]})
			${0} 3       - build step 3 (${BUILD_STEPS[3]})
			${0} 3-      - build from step 3 to the end
			${0} 0-      - build everything
		MSGEND
    exit 0
fi
echo "START:${BUILD_STEPS[$START]}(${START})"
echo "END:${BUILD_STEPS[$END]}(${END})"

I=${START}
while [ ${I} -lt ${END} ]
do
    eval ${BUILD_STEPS[${I}]}
    echo "${I}" > LAST_STEP
    I=$(( ${I} + 1 ))
done

