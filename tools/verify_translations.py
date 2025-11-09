#!/usr/bin/python3
# Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

import argparse
import collections
import json
import logging
import shutil
import subprocess
import sys
import textwrap
from pathlib import Path

logging.basicConfig(level=logging.DEBUG, format="%(levelname)s: %(message)s")
logger = logging.getLogger(__name__)
handler = logging.StreamHandler(sys.stdout)
logger.addHandler(handler)


# note: ripgrep is required for this tool
def detect_duplicate_keys(list_of_pairs):
    key_count = collections.Counter(key for key, value in list_of_pairs)
    duplicate_keys = [key for key, count in key_count.items() if count > 1]

    if duplicate_keys:
        raise ValueError(", ".join(duplicate_keys))


def copy_folder_contents(src_folder: Path, dst_folder: Path):
    dst_folder.mkdir(parents=True, exist_ok=True)

    for file_path in src_folder.glob("*"):
        if file_path.is_file():
            shutil.copy2(file_path, dst_folder / file_path.name)


def write_all_keys_to_file(json_path: Path, output_path: Path):
    with json_path.open() as json_file:
        json_data = json.load(json_file)
        keys = json_data.keys()

    with output_path.open(mode="w") as output_file:
        output_file.write("\n".join(keys))


def validate_data(list_of_pairs: list):
    detect_duplicate_keys(list_of_pairs)
    return dict(list_of_pairs)


def perform_on_files_from_path(json_path: Path, operation):
    json_files = json_path.glob("*.json")
    ret = 0

    for file_path in json_files:
        with file_path.open() as json_file:
            ret |= operation(file_path, json_file)

    return ret


def check_duplicates(file_path: Path, json_file):
    try:
        json.load(json_file, object_pairs_hook=validate_data)
    except ValueError as err:
        duplicate_keys = [key.strip() for key in str(err).split(",") if key.strip()]
        logger.debug(
            "[%s]: duplicate %s: %s",
            file_path.name,
            len(duplicate_keys),
            ", ".join(duplicate_keys),
        )
        return 1
    return 0


def check_empty_entries(file_path: Path, json_file):
    json_data = json.load(json_file)
    empty_entries = [entry for entry, value in json_data.items() if not value]
    if empty_entries:
        logger.debug(
            "[%s]: empty entries %s: %s",
            file_path.name,
            len(empty_entries),
            empty_entries,
        )
        return 1
    return 0


def get_all_keys_from_path(json_path: Path) -> set[str]:
    json_keys = set()

    for file_path in json_path.glob("*.json"):
        with file_path.open() as json_file:
            json_data = json.load(json_file)
            json_keys |= set(json_data.keys())

    return json_keys


def check_missing_entries_from_path(json_path: Path) -> int:
    all_keys = get_all_keys_from_path(json_path)
    ret = 0

    for file_path in json_path.glob("*.json"):
        with file_path.open() as json_file:
            json_data = json.load(json_file)
            missing_keys_in_file = all_keys - set(json_data.keys())

            if missing_keys_in_file:
                with (file_path.with_suffix(".pattern")).open("w") as pattern_file:
                    pattern_file.write("\n".join(missing_keys_in_file))
                ret = 1

    return ret


def fix_json(dst_path: Path):
    json_data = json.loads(dst_path.read_text())
    dst_path.write_text(json.dumps(json_data, indent=4, sort_keys=True))


def fix_jsons(json_dst_path: Path):
    if not json_dst_path.exists():
        json_dst_path.mkdir(parents=True)

    for file_path in json_dst_path.glob("*.json"):
        fix_json(file_path)

    logger.debug("Translation files fixed")


def verify_keys_code_usage(pattern_src_path: Path, pattern_file=None):
    unused_keys = set()
    used_keys = set()

    if pattern_file is None:
        file_list = list(pattern_src_path.glob("*.pattern"))
    else:
        pattern_file_path = pattern_src_path / pattern_file
        if not pattern_file_path.exists():
            raise ValueError(f"Pattern file {pattern_file_path} not found.")
        file_list = [pattern_file_path]

    for pattern_path in file_list:
        with pattern_path.open("r") as file:
            lines = [line.strip() for line in file if line.strip()]
            rg_result = subprocess.run(
                [
                    "rg",
                    "-f",
                    str(pattern_path),
                    "-g",
                    f"!{pattern_src_path}",
                    "-T",
                    "json",
                    "..",
                ],
                stdout=subprocess.PIPE,
            ).stdout.decode("UTF-8")

            for line in lines:
                if line in rg_result:
                    used_keys.add(line)
                else:
                    unused_keys.add(line)

        pattern_path.unlink()

    return unused_keys, used_keys


def remove_unused_keys(json_dst_path: Path, unused_keys: set):
    if not json_dst_path.exists():
        json_dst_path.mkdir(parents=True)

    for file in json_dst_path.glob("*.json"):
        json_data = json.loads(file.read_text())
        for key in unused_keys:
            json_data.pop(key, None)

        temp_path = file.with_suffix(".tmp")
        temp_path.write_text(json.dumps(json_data, indent=4, sort_keys=True))

        shutil.move(str(temp_path), str(file))

    logger.debug("Translation files cleaned up from unused keys")


def get_missing_and_used_keys_for_files(json_path: Path, used_keys: set):
    ret = 0
    dir_list = [x.name for x in json_path.glob("*.json")]

    # iterate to find missing keys
    for file in dir_list:
        file_path = json_path / file
        with file_path.open() as json_file:
            json_data = json.load(json_file)
            missing_keys_in_file = used_keys.difference(set(json_data))

            if missing_keys_in_file:
                logger.debug(
                    "[%s]: missing and used %s: %s",
                    file,
                    len(missing_keys_in_file),
                    sorted(missing_keys_in_file),
                )
                ret |= 1
    return ret


def main(cli_args):
    ret = 0
    src_path = Path(cli_args.src)
    dst_path = Path(cli_args.dst) if cli_args.dst else None

    if cli_args.fix:
        copy_folder_contents(src_path, dst_path)
        fix_jsons(dst_path)

        # check for usage of English.json entries in the code
        write_all_keys_to_file(dst_path / "English.json", dst_path / "English.keys")
        not_used_keys, _ = verify_keys_code_usage(dst_path, "English.keys")
        if not_used_keys:
            logger.critical(
                "unused english keys: %s: %s", len(not_used_keys), not_used_keys
            )

        remove_unused_keys(dst_path, not_used_keys)
        missing_not_used_keys, missing_used_keys = verify_keys_code_usage(src_path)
        ret |= get_missing_and_used_keys_for_files(src_path, missing_used_keys)
        remove_unused_keys(dst_path, missing_not_used_keys)

    ret |= perform_on_files_from_path(src_path, check_empty_entries)
    ret |= perform_on_files_from_path(src_path, check_duplicates)
    ret |= check_missing_entries_from_path(src_path)

    for file in src_path.glob("*.pattern"):
        file.unlink()

    return ret


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        prog="verify_translations",
        description="Script for checking the inconsistency of lang jsons",
        formatter_class=argparse.RawTextHelpFormatter,
    )

    parser.add_argument(
        "-s",
        "--src",
        metavar="path",
        type=Path,
        help="source path to the json files",
        required=True,
    )
    parser.add_argument(
        "--fix",
        action="store_true",
        help=textwrap.dedent(
            """\
        fix the translation files: remove duplicates, remove unused keys and sort
        WARNING! this will overwrite your destination files!

        Use with caution!"""
        ),
    )
    parser.add_argument(
        "-d",
        "--dst",
        metavar="path",
        type=Path,
        help="destination path for the fixed json files",
    )
    parser.add_argument("-v", "--verbose", action="store_true")

    args = parser.parse_args()

    if args.fix and not args.dst:
        parser.error("The destination path must be specified")

    logger.setLevel(logging.DEBUG if args.verbose else logging.CRITICAL)

    error_code = main(args)
    if error_code:
        logging.error("Verification failed")
    sys.exit(error_code)
