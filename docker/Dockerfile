FROM ubuntu:eoan
MAINTAINER radoslaw.wicik@mudiata.com
# basic build system for PurePhoneOs

ADD gcc-arm-none-eabi-9-2019-q4-major-x86_64-linux.tar.bz2 /usr/local/

ADD cmake-3.17.0-Linux-x86_64.tar.gz /usr/local/

RUN apt-get update && \
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
        bash-completion

ADD .bashrc /home/docker/

ENV PATH="/usr/local/cmake-3.17.0-Linux-x86_64/bin:/usr/local/gcc-arm-none-eabi-9-2019-q4-major/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"
ENV TERM="xterm-256color"
