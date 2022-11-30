#!/usr/bin/python3
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# import required module
import argparse
import sys
import json
import logging

log = logging.getLogger(__name__)
logging.basicConfig(format='%(asctime)s [%(levelname)s]: %(message)s', level=logging.INFO)


def get_databases_for_product(input_path, product) -> list:
    try:
        with open(input_path, 'r') as src:
            databases_json_data = json.load(src)
            return databases_json_data[product]
    except Exception as e:
        log.error(e)
        return []


def write_databases_to_json(output_path, databases) -> int:
    try:
        with open(output_path, 'r+') as dst:
            version_json_data = json.load(dst)
            version_json_data.update(databases)
            dst.seek(0)
            json.dump(version_json_data, dst, indent=4)
            return 0
    except Exception as e:
        log.error(e)
        return -1


def main() -> int:
    parser = argparse.ArgumentParser(description='Append databases.json to version.json')
    parser.add_argument('--input_path',
                        metavar='databases_json_path',
                        type=str,
                        help='path to databases.json file',
                        required=True)

    parser.add_argument('--output_path',
                        metavar='version_json_path',
                        type=str,
                        help='path to version.json file',
                        required=True)

    parser.add_argument('--product',
                        metavar='BellHybrid/PurePhone',
                        type=str,
                        help='product which database entries should be loaded',
                        required=True)

    args = parser.parse_args()

    databases = get_databases_for_product(args.input_path, args.product)
    if not databases:
        return -1

    ret = write_databases_to_json(args.output_path, databases)

    return ret


if __name__ == "__main__":
    sys.exit(main())
