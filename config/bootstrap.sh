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
ARM_GCC="gcc-arm-none-eabi-9-2019-q4-major"
ARM_GCC_PACKAGE_FILE="${ARM_GCC}-x86_64-linux.tar.bz2"
ARM_GCC_SOURCE_LINK="https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2019q4/gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2?revision=108bd959-44bd-4619-9c19-26187abf5225&la=en&hash=E788CE92E5DFD64B2A8C246BBA91A249CB8E2D2D"
ARM_GCC_PATH_VAR="${ARM_GCC//-/_}"

CMAKE_DIR="cmake-3.17.0-Linux-x86_64"
CMAKE_SOURCE_LINK="https://github.com/Kitware/CMake/releases/download/v3.17.0/cmake-3.17.0-Linux-x86_64.tar.gz"
CMAKE_PKG="${CMAKE_DIR}.tar.gz"
CMAKE_PATH_VAR=${CMAKE_DIR//[-.]/_}



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
    echo -e "Install style checking hooks"
    options=('Automatic style update.'
             'Just notify me about style errors but do not apply it.'
             'Let me do it manually.'
             )
    CUR_PS3="$PS3"
    PS3="Please select a script:"
    select OPT in "${options[@]}"; do
        case ${REPLY} in 
            1 )
                HOOK="pre-commit.hook"
                break
                ;;
            2 )
                HOOK="pre-commit-check-only.hook"
                break
                ;;
            3 )
                PS3=$CUR_PS3
                return
                ;;
            * )
                echo "invalid option \"${REPLY}\""
                ;;
        esac
    done

    L_GIT_DIR=$(git rev-parse --show-toplevel)
    ln -sf ${L_GIT_DIR}/config/${HOOK} ${L_GIT_DIR}/.git/hooks/pre-commit
    PS3=$CUR_PS3
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

function setup_env_cmake() {
    echo -e "\e[32m${FUNCNAME[0]}\e[0m"
    cat env.cmake.sample | sed "s:<ARM_GCC_HOME>:${HOME}/${ARM_GCC}/bin:" > env.cmake
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
        "setup_env_cmake"
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

