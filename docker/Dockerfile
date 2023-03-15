FROM ubuntu:jammy AS builder
ARG TOOLS_INSTALL_PATH="/usr/local"

ARG ARM_GCC_VERSION="10.3-2021.10"
ARG ARM_GCC_FILENAME=gcc-arm-none-eabi-${ARM_GCC_VERSION}-x86_64-linux
ARG ARM_GCC_FOLDER=gcc-arm-none-eabi-${ARM_GCC_VERSION}

ARG CMAKE_VERSION="3.21.3"
ARG CMAKE_FILENAME="cmake-${CMAKE_VERSION}-linux-x86_64"

ARG CODEQL_VERSION="2.11.1"
ARG CODEQL_NAME="codeql"
ARG CODEQL_FULLNAME="${CODEQL_NAME}-linux64"
ARG CODEQL_PKG="${CODEQL_NAME}.zip"

RUN ln -fs /usr/share/zoneinfo/Europe/Warsaw /etc/localtime
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get dist-upgrade -y && \
    apt-get install -y  apt-utils software-properties-common build-essential ca-certificates clang-format-12 clang-tidy-12 \
                        curl doxygen g++-10 gcc-10 gdb git libgtkmm-3.0-dev libblkid-dev libfdisk-dev libfuse-dev \
                        mtools ninja-build pkg-config pulseaudio libpulse0 libpulse-dev python3-magic python3-pip python3-requests \
                        rhash rsync tzdata tar wget zip fdisk luarocks && \
    apt-get remove ccache -y


RUN luarocks install ldoc
ADD assets/requirements.txt /tmp/
RUN pip3 install -r /tmp/requirements.txt

WORKDIR /tmp
RUN wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/${ARM_GCC_VERSION}/${ARM_GCC_FILENAME}.tar.bz2 && tar -xf ${ARM_GCC_FILENAME}.tar.bz2 -C ${TOOLS_INSTALL_PATH}
RUN wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/${CMAKE_FILENAME}.tar.gz && tar -xf ${CMAKE_FILENAME}.tar.gz -C ${TOOLS_INSTALL_PATH}
RUN wget https://github.com/github/codeql-cli-binaries/releases/download/v${CODEQL_VERSION}/${CODEQL_FULLNAME}.zip && unzip ${CODEQL_FULLNAME}.zip -d ${TOOLS_INSTALL_PATH}
RUN wget https://github.com/ccache/ccache/releases/download/v4.7/ccache-4.7-linux-x86_64.tar.xz && tar -xvf ccache-4.7-linux-x86_64.tar.xz  && cd ccache-4.7-linux-x86_64/ &&  make install

WORKDIR /home/builder
RUN groupadd -r builder && \
    useradd --no-log-init -r -g builder builder && \
    chown -R builder:builder /home/builder

ENV PATH="${TOOLS_INSTALL_PATH}/${CODEQL_NAME}:${TOOLS_INSTALL_PATH}/${ARM_GCC_FOLDER}/bin:${TOOLS_INSTALL_PATH}/${CMAKE_FILENAME}/bin:$PATH"
RUN echo "export PATH=${PATH}" >> /etc/profile

USER builder

FROM builder as jenkins-runner

USER root
RUN apt-get update && \
    apt-get install -y \
        openssh-server \
        openjdk-11-jdk

#Docker drops audit-related capabilities, removing from pam requirements
RUN sed -i 's|session    required     pam_loginuid.so|session    optional     pam_loginuid.so|g' /etc/pam.d/sshd

RUN sed -i /etc/ssh/sshd_config \
        -e 's/#PermitRootLogin.*/PermitRootLogin no/' \
        -e 's/#RSAAuthentication.*/RSAAuthentication yes/'  \
        -e 's/#PasswordAuthentication.*/PasswordAuthentication no/' \
        -e 's/#SyslogFacility.*/SyslogFacility AUTH/' \
        -e 's/#LogLevel.*/LogLevel INFO/'

RUN mkdir -p /var/run/sshd

RUN adduser --quiet --gecos '' --disabled-password --uid 6666 jenkins

COPY assets/.ssh/authorized_keys /home/jenkins/.ssh/authorized_keys
RUN chown -R jenkins:jenkins /home/jenkins/.ssh

EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]