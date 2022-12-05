#!/usr/bin/env python3
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import os
import argparse
import logging
import sys
import tarfile
import json

log = logging.getLogger(__name__)
logging.basicConfig(format='%(asctime)s [%(levelname)s]: %(message)s', level=logging.INFO)


def main() -> int:
    parser = argparse.ArgumentParser(description='Create update package for MuditaOS')
    parser.add_argument('--product',
                        metavar='product',
                        type=str,
                        help='target product: BellHybrid/PurePhone',
                        required=True)

    parser.add_argument('--system_path',
                        metavar='system_path',
                        type=str,
                        help='path to the sysroot/system_a catalog',
                        required=True)

    parser.add_argument('--output_path',
                        metavar='db_path',
                        type=str,
                        help='destination path for the update package',
                        required=True)

    args = parser.parse_args()

    version = None
    json_path = os.path.join(args.system_path, "version.json")

    if os.path.exists(json_path):
        with open(json_path, "r") as json_file:
            json_data = json.load(json_file)
            version = json_data["version"]["version"]
    else:
        log.error("version.json does not exist!")
        return 1

    if version is None:
        log.error("Wrong version or damaged version.json")
        return 1

    output_name = f"{args.product}-{version}-RT1051-Update.tar"
    output_file_path = os.path.join(args.output_path, output_name)

    assets_to_archive = ["bin", "scripts", "assets", "data", "db", "version.json"]
    assets_with_path = [os.path.join(args.system_path, asset) for asset in assets_to_archive]

    if not os.path.exists(args.output_path):
        os.makedirs(args.output_path, exist_ok=True)

    if os.path.exists(output_file_path):
        os.remove(output_file_path)

    with tarfile.open(output_file_path, "w") as tar:
        for asset in assets_with_path:
            tar.add(asset, arcname=os.path.basename(asset))

    return 0


if __name__ == "__main__":
    sys.exit(main())
