FROM ubuntu:eoan
MAINTAINER radoslaw.wicik@mudiata.com
# basic build system for PurePhoneOs

RUN ln -fs /usr/share/zoneinfo/Europe/Warsaw /etc/localtime && \
    export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get full-upgrade -y && \
    apt-get install -y \
        binutils \
        wget \
        git \
        make \
        pkg-config \
        gtkmm-3.0 \
        gcc-9 \
        g++-9 \
        portaudio19-dev \
        build-essential \
        vim \
        bash-completion \
        clang-format-9 \
        apt-utils \
        locales \
        tzdata && \
    locale-gen pl_PL.UTF-8 \
               en_US.UTF-8 \
               de_DE.UTF-8 \
               es_ES.UTF-8 && \
    dpkg-reconfigure --frontend noninteractive tzdata 

ADD gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2 /usr/local/

ADD cmake-3.17.0-Linux-x86_64.tar.gz /usr/local/

ENV PATH="/usr/local/cmake-3.17.0-Linux-x86_64/bin:/usr/local/gcc-arm-none-eabi-9-2019-q4-major/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
ENV TERM="xterm-256color"

ADD .bashrc /home/docker/

COPY entrypoint.sh /entrypoint.sh


ENTRYPOINT ["/entrypoint.sh"]





