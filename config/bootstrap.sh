#!/bin/bash -e
#####################################################################
#
#                   !!! WARNING !!!
#
# this script was written for `raw` ubuntu 18.10 64b
# optionaly consider installing ccache:
#     `sudo apt install ccache`
# it's support is added to default ccache

function installHooks(){
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
    ln -sf ${L_GIT_DIR}/config/prepare-commit-msg.hook ${L_GIT_DIR}/.git/hooks/prepare-commit-msg
    PS3=$CUR_PS3
}

function addIgnoreRevsForBlame() {
    git config blame.ignoreRevsFile .gitblameignorerevs
}

echo "Install neccessary packages"
sudo apt update && sudo apt dist-upgrade -y
sudo apt install -y binutils wget git make pkg-config gtkmm-3.0 gcc-8 g++-8 portaudio19-dev
sudo apt install -y --reinstall build-essential

# names taken from catalogs in tar file
ARM_GCC='gcc-arm-none-eabi-8-2019-q3-update'
CMAKEV='cmake-3.15.5-Linux-x86_64'

echo "Setup gcc, cmake etc..."

echo "Download armgcc"
[[ ! -f $ARM_GCC ]] &&
    wget --no-verbose -O ${ARM_GCC}.tgz https://developer.arm.com/-/media/Files/downloads/gnu-rm/8-2019q3/RC1.1/gcc-arm-none-eabi-8-2019-q3-update-linux.tar.bz2?revision=c34d758a-be0c-476e-a2de-af8c6e16a8a2?product=GNU%20Arm%20Embedded%20Toolchain,64-bit,,Linux,8-2019-q3-update
# untar to HOME
[[ ! -d ${HOME}/${ARM_GCC} ]] &&
    tar -xf ${ARM_GCC}.tgz -C ${HOME}/
echo "ARM GCC installed to ${HOME}/${ARM_GCC}"

[[ ! -f ${CMAKEV}.tgz ]] &&
    wget --no-verbose -O ${CMAKEV}.tgz https://github.com/Kitware/CMake/releases/download/v3.15.5/cmake-3.15.5-Linux-x86_64.tar.gz
[[ ! -d ${HOME}/${CMAKEV} ]] &&
    tar -xf ${CMAKEV}.tgz -C ${HOME}/

echo "Set path for cmake"
echo 'export PATH="${HOME}/cmake-3.15.5-Linux-x86_64/bin:$PATH"' >> ~/.bashrc
export PATH="${HOME}/${CMAKEV}/bin:$PATH"

echo "CMAKEV installed to ${HOME}/${CMAKEV} and set in PATH"

echo "set gcc-8 as default alternative (instead of default gcc-7 in ubuntu)"
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 800 --slave /usr/bin/g++ g++ /usr/bin/g++-8

installHooks
addIgnoreRevsForBlame

echo "Removing downloaded: ${CMAKEV}.tgz and ${ARM_GCC}.tgz is on you :)"
echo "Please mind that running this script multiple times will add ${CMAKEV} to your ~/.bashrc (end of file) multiple times"
