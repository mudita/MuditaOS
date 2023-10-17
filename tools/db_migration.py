#!/usr/bin/python3
# Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import os
import uuid
import sqlite3
from argparse import ArgumentParser
from pathlib import Path
import sys
import datetime
import json
import shutil
import traceback
import itertools

# Constants
up_script = "up.sql"
down_script = "down.sql"
devel_script = "devel.sql"
meta_file = ".meta"
databases_set = "databases.json"
env_file = "dbm_env.ini"

license_header = f"-- Copyright (c) 2017-{datetime.date.today().year}, Mudita Sp. z.o.o. All rights reserved.\n" \
                 "-- For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md\n\n"

cli = ArgumentParser()
subparsers = cli.add_subparsers(dest="subcommand")


def subcommand(args=[], parent=subparsers):
    def decorator(func):
        parser = parent.add_parser(func.__name__, description=func.__doc__)
        for arg in args:
            parser.add_argument(*arg[0], **arg[1])
        parser.set_defaults(func=func)

    return decorator


def argument(*name_or_flags, **kwargs):
    return [*name_or_flags], kwargs


class RevisionMetadata:
    _key_id = "id"
    _key_date = "date"
    _key_message = "message"
    _key_parent = "parent"
    file_name = ".meta"

    def __init__(self, id, date, message, parent):
        self.set = {RevisionMetadata._key_id: str(id), RevisionMetadata._key_date: date,
                    RevisionMetadata._key_message: message, RevisionMetadata._key_parent: parent}

    def id(self):
        return self.set[RevisionMetadata._key_id]

    def parent(self):
        return self.set[RevisionMetadata._key_parent]

    def message(self):
        return self.set[RevisionMetadata._key_message]

    @classmethod
    def from_file(cls, path: Path):
        with open(path, "r") as f:
            raw = json.load(f)
            return cls(raw[cls._key_id], raw[cls._key_date], raw[cls._key_message], raw[cls._key_parent])

    def dump_to_file(self, path: Path):
        with path.open('a') as file:
            file.write(json.dumps(self.set, indent=1))


class ConstRevisionEntry:
    def __init__(self, dir: Path):
        self.dir = dir
        self.metadata = RevisionMetadata.from_file(dir / RevisionMetadata.file_name)

    def read_sql(self):
        lines_to_skip = license_header.count('\n')

        with open(self.dir / up_script) as f:
            up = ''.join(f.readlines()[lines_to_skip:])
        with open(self.dir / down_script) as f:
            down = ''.join(f.readlines()[lines_to_skip:])
        try:
            with open(self.dir / devel_script) as f:
                devel = ''.join(f.readlines()[lines_to_skip:])
        except OSError:
            devel = None
        return up, down, devel


class RevisionEntry:
    def __init__(self, base_dir: Path, message: str):
        self.id = uuid.uuid4()
        self.base_dir = base_dir
        self.dir = base_dir / "{id}_{message}".format(id=str(self.id)[:8], message=message.replace(' ', '_'))
        self.date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        self.message = message
        latest_revision = get_latest_revision(self.base_dir)
        self.metadata = RevisionMetadata(self.id, self.date, self.message,
                                         latest_revision.metadata.id() if latest_revision else 0)

    def spawn(self):
        Path.mkdir(self.dir, exist_ok=True, parents=True)
        self.metadata.dump_to_file(self.dir / RevisionMetadata.file_name)
        self._build_sql_template()

    def _build_sql_template(self):
        with (self.dir / up_script).open('w') as file:
            file.write(license_header + self._sql_header())

        with (self.dir / down_script).open('w') as file:
            file.write(license_header + self._sql_header())

        with (self.dir / devel_script).open('w') as file:
            file.write(license_header + self._sql_header())

    def _sql_header(self):
        return f'-- Message: {self.message}\n' \
               f'-- Revision: {self.id}\n' \
               f'-- Create Date: {self.date}\n\n' \
               f'-- Insert SQL here\n'


class DatabaseSet:
    def __init__(self, path: Path):
        self.key_db_version = "version"
        self.key_db_name = "name"
        self.key_db_array = "databases"

        with open(path, "r") as f:
            self.set = json.load(f)

        self.path = path
        self.product = list(self.set.keys())[0]

    def get_database_version(self, db_name: str):
        v = next(
            d[self.key_db_version] for d in self.set[self.product][self.key_db_array] if
            d[self.key_db_name] == db_name)
        return int(v)

    def db_array(self):
        return self.set[self.product][self.key_db_array]

    def list_databases_by_name(self):
        return set([database["name"] for database in self.db_array()])

    def modify_database_version(self, db_name: str, version: int):
        entry = next(d for d in self.set[self.product][self.key_db_array] if d[self.key_db_name] == db_name)
        entry[self.key_db_version] = str(version)
        with open(self.path, 'w') as file:
            file.write(json.dumps(self.set, indent=1))


class Migration:
    env_var = "DB_MIGRATION_ENV"
    _rev_base_dir = "current"

    def _get_env(self, path: Path):
        """Tries to fetch environment settings from the given file"""
        with open(path / env_file) as f:
            data = json.load(f)

        data["output_dir"] = Path(data["output_dir"])
        data["dirs"][:] = [Path(e) for e in data["dirs"]]
        data["db_set_dir"] = Path(data["db_set_dir"])
        return data

    def _get_db_set(self):
        return DatabaseSet(self._env["db_set_dir"])

    def _get_database_path(self, db_name):
        return next(d / db_name for d in self._env["dirs"] if (Path(d) / db_name).exists())

    def _invoke_sql(self, db_name, script_name):
        base_dir = self._get_database_path(db_name) / Migration._rev_base_dir
        if not base_dir.exists():
            print("Nothing to invoke")
            return

        rev = get_latest_revision(base_dir)
        execute_db_script(self._env["output_dir"] / f"{db_name}.db", rev.dir / script_name)

    def __init__(self, env_path: Path):
        self._env = self._get_env(env_path) if env_path else self._get_env(Path(os.environ.get(Migration.env_var)))

        self.db_names = [os.listdir(d) for d in self._env["dirs"]]
        self.db_names = list(itertools.chain(*self.db_names))

    def upgrade(self, db_name, rev, devel):
        print(f"Upgrading '{db_name}', devel features: {devel}")

        db_path = self._get_database_path(db_name)

        Path.mkdir(self._env["output_dir"], exist_ok=True, parents=True)
        # Remove old database, if exists
        Path.unlink(self._env["output_dir"] / f"{db_name}.db", missing_ok=True)

        # First, migrate using already committed db version from database set file
        version = self._get_db_set().get_database_version(db_name)
        print(f"-> Upgrading to committed version: {version}")
        migrate_database_up(db_name, db_path, self._env["output_dir"], version, devel)

        # Check if 'current' directory exists and apply current revision list
        current_path = db_path / Migration._rev_base_dir
        if not current_path.exists():
            return

        revisions = build_revision_entries(current_path)

        if rev is None:
            print(f"-> Upgrading to the newest available revision: {revisions[-1].metadata.id()}")
            revisions_range = revisions[:]
        else:
            # Upgrade up to the specified revision
            revisions_range = build_revision_entries_up_to(revisions, rev)
            if not revisions_range:
                print(f"-> revision: {rev} does not exist")
                return

            print(f"-> Upgrading to the revision: {rev}")

        for revision in revisions_range:
            meta = revision.metadata
            print(f" -> Running upgrade from {meta.parent()} to {meta.id()}")
            execute_db_script(self._env["output_dir"] / f"{db_name}.db", revision.dir / up_script)

            if devel and os.path.exists(revision.dir / devel_script):
                execute_db_script(self._env["output_dir"] / f"{db_name}.db", revision.dir / devel_script)

    def install(self, devel):
        shutil.rmtree(self._env["output_dir"], ignore_errors=True)
        Path.mkdir(self._env["output_dir"], exist_ok=True, parents=True)

        databases_to_migrate = self._get_db_set().list_databases_by_name().intersection(self.db_names)

        print(f"Database set to be upgraded and installed: {databases_to_migrate}")
        for db_name in databases_to_migrate:
            self.upgrade(db_name, None, devel)

        # Populate output dir with migration scripts, skip 'devel.sql' scripts
        for d in self._env["dirs"]:
            shutil.copytree(d, self._env["output_dir"] / "migration", dirs_exist_ok=True,
                            ignore=shutil.ignore_patterns(devel_script))

    def commit(self, db_name):
        db_path = self._get_database_path(db_name)
        current_path = db_path / Migration._rev_base_dir
        upgrade_version = self._get_db_set().get_database_version(db_name) + 1

        print(f"Committing database '{db_name}':")

        if not current_path.exists():
            print("->Nothing to commit")
            return

        # Prepare new version directory structure
        version_path = db_path / str(upgrade_version)
        Path.mkdir(db_path / version_path, exist_ok=True, parents=True)

        merge_sql_from_dir(current_path, db_path / version_path)

        self._get_db_set().modify_database_version(db_name, upgrade_version)

        shutil.rmtree(current_path)

        print(f"->New version generated from commit: {upgrade_version}")

    def commit_all(self):
        for db_name in self._get_db_set().list_databases_by_name():
            self.commit(db_name)

    def revision(self, db_name, message):
        base_dir = self._get_database_path(db_name) / Migration._rev_base_dir

        Path.mkdir(base_dir, exist_ok=True, parents=True)
        entry = RevisionEntry(base_dir, message)
        entry.spawn()
        print(f"Added new revision: {entry.metadata.id()}")

    def revert(self, db_name):
        self._invoke_sql(db_name, down_script)

    def redo(self, db_name):
        self._invoke_sql(db_name, down_script)
        self._invoke_sql(db_name, up_script)


def build_revision_entries(base: Path):
    """ Builds the list of ConstRevisionEntry entries where each child is placed after its parent.
     Revision_1(id=1,parent=0) -> Revision_2(id=2,parent=1) -> Revision_n(id=n,parent=2)
     """

    metas = []

    for entry in base.iterdir():
        metas.append(ConstRevisionEntry(entry))

    chain = []
    parent_index = 0
    for _ in metas:
        try:
            entry = next(d for d in metas if d.metadata.parent() == parent_index)
            parent_index = entry.metadata.id()
            chain.append(entry)
        except StopIteration:
            break
    return chain


def build_revision_entries_up_to(revisions, rev):
    """ Try to build the list of ConstRevisionEntry entries from the already existing list of revisions up to the
    specified revision. For instance, Revision_1(id=1,parent=0) -> Revision_2(id=2,parent=1) -> Revision_n(id=rev,
    parent=2)
    """
    if next((r for r in revisions if r.metadata.id() == rev), [None]):
        revisions_range = []
        for r in revisions:
            revisions_range.append(r)
            if r.metadata.id() == rev:
                return revisions_range
    else:
        return None


def get_latest_revision(base: Path):
    """Obtains the latest ConstRevisionEntry """
    chain = build_revision_entries(base)
    return None if len(chain) == 0 else chain[-1]


def merge_sql_from_dir(directory: Path, out: Path):
    revisions = build_revision_entries(directory)

    # Merge up/down.sql
    with open(out / up_script, 'w') as up_file, open(out / down_script, 'w') as down_file:
        up_file.write(license_header)
        down_file.write(license_header)

        for rev in revisions:
            print(f"->Merging revision: {rev.metadata.id()}")
            sql_up, _, sql_devel = rev.read_sql()
            up_file.write(sql_up + '\n')

            if sql_devel:
                if not (out / devel_script).exists():
                    with open(out / devel_script, 'w') as devel_file:
                        devel_file.write(license_header)

                with open(out / devel_script, 'a+') as devel_file:
                    devel_file.write(sql_devel + '\n')

        # Down scripts need to be merged in reversed order
        for rev in reversed(revisions):
            _, sql_down, _ = rev.read_sql()
            down_file.write(sql_down + '\n')


def execute_db_script(db_path: Path, script: Path, version: int = None):
    connection = sqlite3.connect(db_path)
    with open(script) as ms:
        connection.executescript(ms.read())
        connection.commit()
        if version:
            connection.execute(f"PRAGMA user_version = {version};")
            connection.commit()
    connection.close()


def migrate_database_up(database: str, migration_path: os.path, dst_directory: os.path, dst_version: int, devel: bool):
    db_name_full = f"{database}.db"
    dst_db_path = dst_directory / db_name_full
    Path(dst_db_path).unlink(missing_ok=True)

    for i in range(dst_version + 1):
        migration_script = migration_path / str(i) / up_script
        devel_script_path = migration_path / str(i) / devel_script
        execute_db_script(dst_db_path, migration_script, i)
        if devel and os.path.exists(devel_script_path):
            execute_db_script(dst_db_path, devel_script_path, i)


@subcommand([argument("-e", "--env", help="where to store environment configuration", required=True, type=Path),
             argument("--dbset", help="location of the file describing database set", required=True, type=Path),
             argument("-o", "--out", help="where to store generated databases", required=True, type=Path),
             argument("--dirs",
                      help="list of migration base directories. It's important to pass product-specific directory as "
                           "a first element on the list",
                      action='append',
                      nargs='*',
                      required=True,
                      type=Path)])
def init(args):
    """Initializes migration environment"""
    env = {"db_set_dir": args.dbset.as_posix(), "output_dir": args.out.as_posix(),
           "dirs": [a[0].as_posix() for a in args.dirs]}
    with open(args.env / env_file, 'w') as f:
        f.write(json.dumps(env, indent=1))


@subcommand([argument("-e", "--env", help="environment location", type=Path),
             argument("--db", help="database name", required=True, type=str),
             argument("-m", "--message", help="revision message", required=True, type=str)])
def revision(args):
    """Creates a new database migration revision"""
    Migration(args.env).revision(args.db, args.message)


@subcommand([argument("-e", "--env", help="environment location", type=Path),
             argument("--db", help="database name", type=str)])
def commit(args):
    """Commits current set of SQL statements and updates database version number"""
    if args.db:
        Migration(args.env).commit(args.db)
    else:
        Migration(args.env).commit_all()


@subcommand(
    [argument("-e", "--env", help="environment location", type=Path),
     argument("-d", "--devel", help="with development schema", default=False)])
def install(args):
    """ Generates database set and then installs it in the specific output directory. It also populates output
    directory with corresponding migration scripts"""
    Migration(args.env).install(args.devel)


@subcommand(
    [argument("-e", "--env", help="environment location", type=Path),
     argument("--db", help="database name", type=str, required=True),
     argument("-r", "--revision", help="target revision", type=str),
     argument("-d", "--devel", help="with development schema", default=False)])
def upgrade(args):
    """ Upgrades database to the specific revision(or the newest one if --revision parameter omitted)"""
    Migration(args.env).upgrade(args.db, args.revision, args.devel)


@subcommand([argument("-e", "--env", help="environment location", type=Path),
             argument("--db", help="database name", type=str, required=True)])
def revert(args):
    """ Runs the (down.sql) for the specified database for the most recent migration"""
    Migration(args.env).revert(args.db)


@subcommand([argument("-e", "--env", help="environment location", type=Path),
             argument("--db", help="database name", type=str, required=True)])
def redo(args):
    """ Runs the (down.sql) and then the (up.sql) for the most recent migration"""
    Migration(args.env).redo(args.db)


def main() -> int:
    args = cli.parse_args()
    if args.subcommand is None:
        cli.print_help()
        return 1
    else:
        try:
            args.func(args)
        except:
            print(traceback.format_exc())
            return 1


if __name__ == "__main__":
    sys.exit(main())
