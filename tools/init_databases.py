#!/usr/bin/python3
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# import required module
import os
import sqlite3
import argparse
import logging
import sys

log = logging.getLogger(__name__)
logging.basicConfig(format='%(asctime)s [%(levelname)s]: %(message)s', level=logging.INFO)


# this helper script creates DBs from SQL schema files

def initialize_database(script_path: os.path, dst_directory: os.path) -> int:
    file_name = os.path.basename(script_path)
    connection = None

    db_name = file_name.split("_0")[0]
    db_name = f"{db_name}.db"
    dst_db_path = os.path.join(dst_directory, db_name)
    log.debug(f"Executing {script_path} script into {dst_db_path} database")

    ret = 0
    try:
        connection = sqlite3.connect(dst_db_path)
        with open(script_path) as fp:
            connection.executescript(fp.read())
        connection.commit()
    except OSError as e:
        log.error(f"System error: {e}")
        ret = 1
    except sqlite3.Error as e:
        log.error(f"[SQLite] {db_name} database error: {e}")
        ret = 1
    finally:
        if connection:
            connection.close()

    return ret


def main() -> int:
    parser = argparse.ArgumentParser(description='Create databases from schema scripts')
    parser.add_argument('--input_path',
                        metavar='schema_path',
                        type=str,
                        help='path to schema scripts',
                        required=True)

    parser.add_argument('--output_path',
                        metavar='db_path',
                        type=str,
                        help='destination path for databases',
                        required=True)

    parser.add_argument('--development',
                        metavar='devel',
                        type=bool,
                        help='with development schema scripts',
                        default=False)

    args = parser.parse_args()

    db_script_files = [
        os.path.join(args.input_path, file)
        for file in os.listdir(args.input_path)
        if os.path.isfile(os.path.join(args.input_path, file)) and ".sql" in file
    ]
    db_script_devel = [file for file in db_script_files if "devel" in file]
    db_script_no_devel = list(set(db_script_files) - set(db_script_devel))

    db_script_devel.sort()
    db_script_no_devel.sort()

    if not os.path.exists(args.output_path):
        os.makedirs(args.output_path, exist_ok=True)

    ret = 0

    for script in db_script_no_devel:
        ret |= initialize_database(script, args.output_path)

    if args.development:
        for script in db_script_devel:
            ret |= initialize_database(script, args.output_path)

    return ret


if __name__ == "__main__":
    sys.exit(main())
