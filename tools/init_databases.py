#!/usr/bin/python3
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

# import required module
import os
import sqlite3
import argparse
import logging
import sys
import json
import shutil
from pathlib import Path

log = logging.getLogger(__name__)
logging.basicConfig(format='%(asctime)s [%(levelname)s]: %(message)s', level=logging.INFO)

databases_json_filename = "databases.json"
scripts_folder_name = "scripts"
migration_folder_name = "migration"


# this helper script creates DBs from SQL schema files
def migrate_database_up(database: str, migration_path: os.path, dst_directory: os.path, dst_version: int, devel: bool):
    connection = None

    db_name_full = f"{database}.db"
    dst_db_path = os.path.join(dst_directory, db_name_full)
    Path(dst_db_path).unlink(missing_ok=True)

    ret = 0
    try:
        connection = sqlite3.connect(dst_db_path)
        log.info(f"\nPerforming up-migration of {database} to {dst_version}")
        for i in range(dst_version + 1):
            migration_script = os.path.join(migration_path, *[database, str(i), "up.sql"])
            devel_script = os.path.join(migration_path, *[database, str(i), "devel.sql"])
            with open(migration_script) as ms:
                connection.executescript(ms.read())
                connection.commit()
                if devel and os.path.exists(devel_script):
                    with open(devel_script) as ds:
                        connection.executescript(ds.read())
                connection.commit()
                connection.execute(f"PRAGMA user_version = {i};")
                connection.commit()

    except OSError as e:
        log.error(f"System error: {e}")
        ret = 1
    except sqlite3.Error as e:
        log.error(f"[SQLite] {database} database error: {e}")
        ret = 1
    finally:
        if connection:
            connection.close()

    return ret


def migrate_database_wrapper(migration_path: os.path, json: json, dst_directory: os.path, devel: bool) -> int:
    product = list(json.keys())[0]
    databases_json = json[product]["databases"]
    databases = os.listdir(migration_path)
    databases_to_migrate = set([database["name"] for database in databases_json]).intersection(databases)

    for database in databases_json:
        name = database["name"]
        if name in databases_to_migrate:
            migrate_database_up(name, migration_path, dst_directory, int(database["version"]), devel)

    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description='Create databases from schema scripts')
    parser.add_argument('--common_path',
                        metavar='common_path',
                        type=str,
                        help='path to common databases scripts',
                        required=True)

    parser.add_argument('--product_path',
                        metavar='product_path',
                        type=str,
                        help='path to product-specific databases scripts',
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

    ret = 0

    json_path = os.path.join(args.product_path, databases_json_filename)
    json_data = None

    if os.path.exists(json_path):
        with open(json_path, "r") as json_file:
            json_data = json.load(json_file)
    else:
        log.error("Json file does not exists!")
        return 1

    if not os.path.exists(args.output_path):
        os.makedirs(args.output_path, exist_ok=True)

    for database_path in [args.common_path, args.product_path]:
        migration_path = os.path.join(database_path, migration_folder_name)
        ret |= migrate_database_wrapper(migration_path, json_data, args.output_path, args.development)
        shutil.copytree(migration_path, os.path.join(args.output_path, migration_folder_name), dirs_exist_ok=True)

    return ret


if __name__ == "__main__":
    sys.exit(main())
