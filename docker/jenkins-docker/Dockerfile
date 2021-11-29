FROM wearemudita/mudita_os_builder:latest

MAINTAINER ops@mudita.com
# Docker runner for MuditaOS builds

USER root

RUN export DEBIAN_FRONTEND=noninteractive 

RUN apt-get update 
RUN apt-get full-upgrade -y 

RUN apt-get install -y \
        openssh-server \
        openjdk-8-jdk

RUN apt-get -qy clean

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

COPY .ssh/authorized_keys /home/jenkins/.ssh/authorized_keys
COPY start-sshd /usr/local/bin/start-sshd
RUN chown -R jenkins:jenkins /home/jenkins/.ssh
RUN cat /etc/profile.d/setup_path.sh >> /home/jenkins/.bashrc

#RUN chmod -x /cmd.sh && \
#    chmod -x /entrypoint.sh

EXPOSE 22

ENTRYPOINT ["/usr/local/bin/start-sshd"]
#ENTRYPOINT ["/bin/sleep", "infinity"]
