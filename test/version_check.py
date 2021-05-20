#!/usr/bin/env python
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import sys
import argparse

from harness import log
from harness.harness import Harness
from harness.interface.error import TestError, Error


def get_device_info(harness):
    body = {}
    return harness.endpoint_request("deviceInfo", "get", body)


def get_version(harness):
    device_info = get_device_info(harness)
    return [device_info["body"]["gitRevision"], device_info["body"]["gitBranch"], device_info["body"]["gitTag"]]


def main():
    parser = argparse.ArgumentParser(description="Script for checking passed git branch, revision, and tag against OS "
                                                 "values")
    parser.add_argument("-p", "--port", type=str, help="serial port/pts name")
    parser.add_argument("-tag", "--git_tag", type=str, help="git tag")
    parser.add_argument("-rev", "--git_revision", type=str, help="git revision")
    parser.add_argument("-b", "--git_branch", type=str, help="git branch")
    args = parser.parse_args()

    if len(sys.argv) == 1 or "/dev" not in args.port:
        log.warning("Port name not passed, trying port name filename from simulator...")
        try:
            file = open("/tmp/purephone_pts_name", "r")
        except FileNotFoundError as err:
            raise TestError(Error.PORT_FILE_NOT_FOUND)

        port_name = file.readline()
        if port_name.isascii():
            log.debug("found {} entry!".format(port_name))
        else:
            print(f'Please pass port name as the parameter')
            raise TestError(Error.PORT_NOT_FOUND)
    else:
        port_name = args.port

    if args.git_tag is None or args.git_revision is None or args.git_branch is None:
        parser.print_help()
        exit()

    harness = Harness(port_name)
    harness.unlock_phone()
    [rev, branch, tag] = get_version(harness)

    if args.git_tag != tag or args.git_branch != branch or args.git_revision != rev:
        raise TestError(Error.VERSION_MISMATCH)
    else:
        log.info("Version correct!")
        exit(0)


if __name__ == "__main__":
    try:
        main()
    except TestError as err:
        log.error(err)
        exit(err.get_error_code())
