# Database migration

This document contains all the required information regarding database structure, migrations and how to use database
migration tool.

## Overview

Here's an example of a database directory structure:

```
products/<product>/services/db/databases/
└── migration
    ├── db_1
    │   └── 0
    ├── db_2
    │   ├── 0
    │   ├── 1
    │   ├── 2
    │   └── current
    │       ├── 023jh23a_changed_table
    │       └── 11k4eff1_new_field
    ├── db_3
    │   └── 0
    ├── db_4
    │   ├── 0
    │   └── 1
    ├── db_5
    │   ├── 0
    │   └── current
    │       ├── 65a9ee3e_add_new_table
    │       └── 34a4e212_add_new_field
    ├── db_6
    │   └── 0
    ├── db_7
    │   ├── 0
    │   └── 1
    ├── db_8
    │   └── 0
    └── db_n
        └── 0
```

Each database has its directory entry. Then each entry
is further subdivided into versions. Each version entry contains two obligatory files: `up.sql`, and `down.sql`.
You may also find an optional `devel.sql` file there.

Each entry that is assigned a version number is read-only. That means,
this version was officially released to the public and cannot be modified.

Folders named `current` contain migrations that are still under development. Each folder may contain many revision
entries,
where each entry links to one migration(even a small one). Such an approach allows for better granularity.
Upon a new official release, these entries will be merged and assigned a version number, making them read-only.

### Migrations

Migrations allow us to evolve database schema over time. Each migration can be applied (up.sql) or reverted (down.sql).
Applying and immediately reverting a migration should leave your database schema unchanged.

#### up.sql

It contains all the required SQL operations to apply migration.

Example:

```
CREATE TABLE posts (
  id SERIAL PRIMARY KEY,
  title VARCHAR NOT NULL,
  body TEXT NOT NULL,
  published BOOLEAN NOT NULL DEFAULT FALSE
)
```

#### down.sql

Similarly, this file contains all the required SQL operations to revert migration.

Example:

```
DROP TABLE posts
```

#### Optional: devel.sql

You can put there SQL operations that you would use during product development. They will be applied
along with the `up.sql`.
> Development features are not applied as part of official releases. They are only being used
> for internal development needs.

### Specifying database version set

When generating the product image, the migration script retrieves the information it needs from the `databases.json`
file.
This file lists the databases used by the product and their specific versions. Such an approach allows for re-usability
of the same database schemas in different products.

An example of such a file is listed below:

```
{
  "product": {
    "databases":[
      {"name": "db_1", "version": "0"},
      {"name": "db_2", "version": "2"},
      {"name": "db_3", "version": "0"},
      {"name": "db_4", "version": "4"},
      {"name": "db_5", "version": "0"},
      {"name": "db_6", "version": "1"},
      {"name": "db_8", "version": "0"},
      {"name": "db_9", "version": "1"},
      {"name": "db_n", "version": "0"}
    ]
  }
}
```

You do not need to modify this file, as all the required modifications are applied
automatically by the migration tool.

## Migration tool

Database migration operations are handled by using the [migration tool](../tools/db_migration.py).

Currently, it supports:

```
init - init migration environment
revision - create a new migration revision
commit - merge all the existing revisions and generate a new frozen database version
upgrade - upgrade the database to the specific revision
install - generates database set and then installs it in the specified output directory
revert - execute down.sql script for the newest migration
redo - execute down.sql and up.sql scripts for the newest migration
```

All the required info regarding specific subcommands can be printed by passing `--help` as its argument, for instance:

```
python3 db_migration.py revision --help                            
usage: db_migration.py revision [-h] [-e ENV] --db DB -m MESSAGE

Creates a new database migration revision

optional arguments:
  -h, --help            show this help message and exit
  -e ENV, --env ENV     environment location
  --db DB               database name
  -m MESSAGE, --message MESSAGE
                        revision message

```

## Adding database migration step-by-step

Below, there is a detailed step-by-step instruction on how to add/update database migration:

1. Add CMake build directory to the env variable: `export DB_MIGRATION_ENV=<build_dir>`. This step is optional but
   highly recommended. Without it, you would need to pass it each time you invoke the migration tool.
2. Execute `python3 db_migration.py revision --db <database_you_want_to_update> -m "Short description of the revision"`.
   This will create a `current` folder(if it doesn't exist already) and place a revision entry under it.
   The revision entry looks like this:
   ``` 
   current
   └── 65a9ee3e_description
        ├── .meta 
        ├── devel.sql
        ├── down.sql
        └── up.sql
   ```
   Its name(`65a9ee3e_description`) is created from the first 8 chars of the assigned unique ID and description string
   that you passed to the command. The script also generated a set of empty SQL files: `up.sql`, `down.sql`
   and `devel.sql`
   that you will have to fill up. `.meta` file contains various info about revisions like creation date, unique
   identifier and more. **Do not modify or remove it!**
3. Add required SQL to `up.sql` and `down.sql`. Remember that `down.sql` should revert all the changes introduced
   by `up.sql`.
4. If you do not need development-specific migration, you can remove `devel.sql`.
5. To apply newly added migration, you can use either:
    * `python3 db_migration.py upgrade --db <database>`
    * `{product}-disk-img` build target
6. You can also upgrade a database to the specific revision by
   invoking `python3 db_migration.py upgrade --db <database> --rev <revision_id>`. Revision ID can be obtained from
   the revision's meta file.
7. Congratulations, you've managed to update the database schema.

## Committing revisions

When the time has come for issuing a new official database release all you have to do is invoke the:

`python3 db_migration.py commit`

This command will do the following steps for each database specified in `databases.json`:

* merge all the existing revisions
* assign a new database version number to them
* copy merged revisions to the newly created version directory
* remove the `current` folder

You can also commit each database manually by invoking the:

`python3 db_migration.py commit --db <database_name>`

> These operations are irreversible.
