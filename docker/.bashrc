# ~/.bashrc: executed by bash(1) for non-login shells.
# see /usr/share/doc/bash/examples/startup-files (in the package bash-doc)
# for examples

# If not running interactively, don't do anything
case $- in
    *i*) ;;
      *) return;;
esac
RED='\e[38;2;255;13;0m'
YELLOW="\e[38;2;255;232;0m"
BLUE="\e[38;2;20;5;210m"
GREEN="\e[38;2;0;222;27m"
ORANGE="\e[38;2;255;100;0m"

function host_color(){
    echo -en "${YELLOW}"
}
function dir_color(){
    echo -en "${GREEN}"
}
function git_color(){
    echo -en "${ORANGE}"
}

# don't put duplicate lines or lines starting with space in the history.
# See bash(1) for more options
HISTCONTROL=ignoreboth

# append to the history file, don't overwrite it
shopt -s histappend

# for setting history length see HISTSIZE and HISTFILESIZE in bash(1)
HISTSIZE=1000
HISTFILESIZE=2000

export GIT_PS1_SHOWSTASHSTATE=1
export GIT_PS1_SHOWDIRTYSTATE=1

# check the window size after each command and, if necessary,
# update the values of LINES and COLUMNS.
shopt -s checkwinsize

# If set, the pattern "**" used in a pathname expansion context will
# match all files and zero or more directories and subdirectories.
#shopt -s globstar

# make less more friendly for non-text input files, see lesspipe(1)
[ -x /usr/bin/lesspipe ] && eval "$(SHELL=/bin/sh lesspipe)"

# enable color support of ls and also add handy aliases
if [ -x /usr/bin/dircolors ]; then
    test -r ~/.dircolors && eval "$(dircolors -b ~/.dircolors)" || eval "$(dircolors -b)"
    alias ls='ls --color=auto'
    #alias dir='dir --color=auto'
    #alias vdir='vdir --color=auto'

    alias grep='grep --color=auto'
    alias fgrep='fgrep --color=auto'
    alias egrep='egrep --color=auto'
fi

# colored GCC warnings and errors
#export GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'

# some more ls aliases
alias ll='ls -alF'
alias la='ls -A'
alias l='ls -CF'

# Add an "alert" alias for long running commands.  Use like so:
#   sleep 10; alert
alias alert='notify-send --urgency=low -i "$([ $? = 0 ] && echo terminal || echo error)" "$(history|tail -n1|sed -e '\''s/^\s*[0-9]\+\s*//;s/[;&|]\s*alert$//'\'')"'

# enable programmable completion features (you don't need to enable
# this, if it's already enabled in /etc/bash.bashrc and /etc/profile
# sources /etc/bash.bashrc).
if ! shopt -oq posix; then
  if [ -f /usr/share/bash-completion/bash_completion ]; then
    . /usr/share/bash-completion/bash_completion
  elif [ -f /etc/bash_completion ]; then
    . /etc/bash_completion
  fi
fi
export gcc_arm_none_eabi_9_2019_q4_major_PATH="/usr/local/gcc-arm-none-eabi-9-2019-q4-major/bin"
PATH="${gcc_arm_none_eabi_9_2019_q4_major_PATH:+${gcc_arm_none_eabi_9_2019_q4_major_PATH}:}${PATH}"
export cmake_3_17_0_Linux_x86_64_PATH="/usr/local/cmake-3.17.0-Linux-x86_64/bin"
PATH="${cmake_3_17_0_Linux_x86_64_PATH:+${cmake_3_17_0_Linux_x86_64_PATH}:}${PATH}"
export PATH


PS1='\[\e[0m\]\n\[$(host_color)\]@pure_phone_builder \[$(dir_color)\]: \w \[\e[0m\]\[\e[33m\]$(__git_ps1 "${ORANGE}: %s")\[\e[0m\]\n\$'

export PS1

