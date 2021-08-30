#!/bin/bash
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

CONTAINER_NAME="wearemudita/mudita_os_builder"
CONTAINER_TAG="latest"
CONTAINER=${CONTAINER_NAME}:${CONTAINER_TAG}
PURE_HOME=`pwd`
STANDARD_OPTIONS="-v `pwd`:${PURE_HOME} --user \"$(id -u):$(id -g)\" --env HOME=${PURE_HOME} -t"

RCFILE="/home/docker/.bashrc"

if [[ $BASH_VERSINFO -lt 4 ]]; then
    echo "Please update your bash to at least version 4"
	echo "your is: ${BASH_VERSINFO}"
    exit 4
fi

function help() {
    cat <<- MSGEND
		This script runs configuration and builds in docker container.
		You don't have to setup entire build environment, just use container.
		 
		Usage:
		${0} login|config|make [additional options]
		 
		    login                                   - login to build container - usually not needed
		    config <target> <build_type> [OPTIONS]  - run ./configure.sh in container
		                                              <target> and <build_type> are required
		                                              OPTIONS are passed to cmake as is
		                                              run '${0} config' without params to see details
		    make|ninja <build_directory> [OPTIONS]        - run 'make' in container. You have to pass directory
		                                              created by '${0} config' command.
		                                              you can pass additional arguments for make linke '-j' or 'VERBOSE=1'
		MSGEND
    exit 0
}

TARGET=$1
shift 
case ${TARGET} in
    "login") 
        CMD="docker run ${STANDARD_OPTIONS} -w ${PURE_HOME} -it --entrypoint /bin/bash ${CONTAINER} --rcfile ${RCFILE}"
        ;;
    "config")
        CMD="docker run ${STANDARD_OPTIONS} -w ${PURE_HOME} --entrypoint ./configure.sh ${CONTAINER}  $@"
        ;;
    "make")
        BUILD_DIR=$1
        shift
        JOBS=$(nproc)
        CMD="docker run ${STANDARD_OPTIONS} -w ${PURE_HOME}/${BUILD_DIR} --entrypoint make ${CONTAINER} -j ${JOBS} $@"
        ;;
    "ninja")
        BUILD_DIR=$1
        shift
        CMD="docker run ${STANDARD_OPTIONS} -w ${PURE_HOME}/${BUILD_DIR} --entrypoint ninja ${CONTAINER} $@"
        ;;
    *)
        help
        ;;
esac

echo ${CMD}
eval ${CMD}


