#!/usr/bin/python3
# Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import collections
import os
import os.path as path
import argparse
import json
import sys
import logging
import textwrap

logging.basicConfig(level=logging.DEBUG, format='%(levelname)s: %(message)s')
logger = logging.getLogger(__name__)
handler = logging.StreamHandler(sys.stdout)
logger.addHandler(handler)


def detect_duplicate_keys(list_of_pairs: list):
    key_count = collections.Counter(k for k, v in list_of_pairs)
    duplicate_keys = ', '.join(k for k, v in key_count.items() if v > 1)

    if duplicate_keys:
        raise ValueError(duplicate_keys)


def validate_data(list_of_pairs: list):
    detect_duplicate_keys(list_of_pairs)
    # More detection, each of them will raise exception upon invalid
    # data
    return dict(list_of_pairs)


def perform_on_files_from_path(json_path: path, operation):
    dir_list = os.listdir(json_path)
    ret = 0

    for file in dir_list:
        file_path = path.join(json_path, file)
        with open(file_path) as json_file:
            ret |= operation(file_path, json_file)
    return ret


def check_duplicates(file_path: path, json_file):
    try:
        _ = json.load(json_file, object_pairs_hook=validate_data)
    except ValueError as e:
        dup_list = str(e).split(',')
        logger.warning(f"[{path.basename(file_path)}]: duplicate {len(dup_list)}: {dup_list}")
        return 1
    return 0


def check_empty_entries(file_path: path, json_file):
    json_data = json.load(json_file)
    empty_entries = [entry for entry in json_data if len(str(json_data[entry])) == 0]
    if empty_entries:
        logger.warning(f"[{path.basename(file_path)}]: empty entries {len(empty_entries)}: {empty_entries}")
        return 1
    return 0


def get_all_keys_from_path(json_path: path):
    dir_list = os.listdir(json_path)
    json_keys = []

    # iterate to get all possible keys and check for key duplicates
    for file in dir_list:
        file_path = path.join(json_path, file)

        with open(file_path) as json_file:
            json_data = json.load(json_file)
            json_keys.append(set(json_data))

    return set.union(*json_keys)


def check_missing_entries_from_path(json_path: path):
    ret = 0
    dir_list = os.listdir(json_path)
    all_keys = get_all_keys_from_path(json_path)

    # iterate to find missing keys
    for file in dir_list:
        file_path = path.join(json_path, file)
        with open(file_path) as json_file:
            json_data = json.load(json_file)
            missing_keys_in_file = all_keys.difference(set(json_data))
            if missing_keys_in_file:
                logger.warning(f"[{file}]: missing {len(missing_keys_in_file)}: {sorted(missing_keys_in_file)}")
                ret |= 1
    return ret


def fix_jsons(json_src_path: path, json_dst_path: path):
    dir_list = os.listdir(json_src_path)
    for file in dir_list:
        src_file_path = path.join(json_src_path, file)
        dst_file_path = path.join(json_dst_path, file)
        if not path.exists(json_dst_path):
            os.makedirs(json_dst_path)

        with open(src_file_path) as json_file, open(dst_file_path, 'w') as outfile:
            json_data = json.load(json_file)
            json.dump(json_data, outfile, indent=4, sort_keys=True)

    logger.info("Translation files fixed")


def main(args):
    ret = 0
    if args.fix:
        fix_jsons(args.src, args.dst)

    ret |= check_missing_entries_from_path(args.src)
    ret |= perform_on_files_from_path(args.src, check_empty_entries)
    ret |= perform_on_files_from_path(args.src, check_duplicates)
    return ret


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog='verify_translations',
        description='Script for checking the inconsistency of lang jsons',
        formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('-s', '--src', metavar='path', help="source path to the json files", required=True)
    parser.add_argument('--fix', action='store_true', help=textwrap.dedent('''\
        fix the translation files: remove duplicates and sort
        WARNING! this will overwrite your destination files!
        
        Use with caution!'''))
    parser.add_argument('-d', '--dst', metavar='path', help="destination path for the fixed json files")
    parser.add_argument('-v', '--verbose', action='store_true')

    args = parser.parse_args()

    if args.fix and not args.dst:
        parser.error("The destination path must be specified")
        sys.exit(1)

    if args.verbose:
        logger.setLevel(logging.DEBUG)
    else:
        logger.setLevel(logging.CRITICAL)

    error_code = main(args)
    if error_code:
        logging.error("Verification failed")
    sys.exit(error_code)
