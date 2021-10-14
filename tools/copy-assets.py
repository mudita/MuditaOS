#!/usr/bin/env python3

'''
Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
'''

import argparse
import json
from pathlib import Path
import sys
import os


def eprint(**kwargs):
    print(kwargs, file=sys.stderr)


def existing_dir(path):
    path = Path(path)

    if path.is_dir():
        return path
    else:
        raise argparse.ArgumentTypeError(f'"{path}" is not a existing directory.')


def parse_args():
    parser = argparse.ArgumentParser()

    parser.add_argument('source_dir', type=existing_dir, help='Assets files source directory.')
    parser.add_argument('destination_dir', type=Path, help='Assest destination directory.')
    parser.add_argument('assets_def', type=argparse.FileType('r'), help='JSON file defining files to copy.')
    parser.add_argument('-d', '--devel', action='store_true', help='Copy development asssets.')

    return parser.parse_args()


def parse_definition(args):
    definition = json.load(args.assets_def)

    definition['srcDir'] = args.source_dir
    definition['dstDir'] = args.destination_dir
    definition['copyDevel'] = args.devel

    return definition


def populate_dst(dst_dir, files, copy_devel):
    if dst_dir.exists() and not dst_dir.is_dir():
        eprint(f'"{dst_dir}" exists and is not a directory.')
        sys.exit(1)

    dst_dir.mkdir(parents=True, exist_ok=True)

    exclude_devel = '' if copy_devel else '--exclude \"*-devel*\"'
    file_list = ' '.join(str(file) for file in files)

    os.system(f'rsync -qavu --delete {exclude_devel} {file_list} {dst_dir}')


def rename_devel_files(assets_dst_dir):
    os.system(f'find {assets_dst_dir} -name "*-devel*" | sed "s,\\(.*\\)-devel\\(.*\\),& \\1\\2," | xargs --no-run-if-empty -L1 mv')


def copy_assets(definition):
    assets_src_dir = definition['srcDir']
    assets_dst_dir = definition['dstDir']
    copy_devel = definition['copyDevel']

    for dst_dir, files in definition['assets'].items():
        dst_dir = assets_dst_dir / dst_dir
        files = [(assets_src_dir / file) for file in files]
        populate_dst(dst_dir, files, copy_devel)

    if copy_devel:
        rename_devel_files(assets_dst_dir)


def main():
    args = parse_args()
    definition = parse_definition(args)
    copy_assets(definition)


if __name__ == '__main__':
    main()
