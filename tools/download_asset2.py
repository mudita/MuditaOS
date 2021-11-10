#!/usr/bin/python3
# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

'''
class to download assets listed in json file
additional classes can be added to i.e. just copy files from some other location
which is super simple as it uses json file for description

{
    "comment": "This is kiss structure - asset path & sha",         <-- random comment in file if you wish
    "assets": [
        {
            "name": "./fonts/bell/gt_pressura_regular_38.mpf",      <-- name of our file to download in repo
            "output": "fonts/gt_pressura_regular_38.mpf",           <-- output: where should be and how shall be called the file
            "ref": "fd168040c5d1216d457e6cf223e8ea9bb76bf7b"        <-- from what ref shall we download the file
        },
    ...
}

Github downloader essentially:
-> connects to api -> loads json -> check if file for `sha` exists locally -> downloads file if not exists -> copy file from cache
'''
import subprocess
import json
from functools import lru_cache
from tqdm import tqdm
from ghapi.all import GhApi
from pathlib import Path
from fastcore.net import HTTP404NotFoundError, HTTP403ForbiddenError
from base64 import b64decode
import logging


log = logging.getLogger(__name__)
log.setLevel(logging.DEBUG)


class TqdmLoggingHandler(logging.Handler):
    '''
    required for pretty logs with tqdm
    '''
    def __init__(self, level=logging.NOTSET):
        super().__init__(level)

    def emit(self, record):
        try:
            msg = self.format(record)
            tqdm.write(msg)
            self.flush()
        except Exception:
            self.handleError(record)


log.addHandler(TqdmLoggingHandler())


def getToken():
    '''
    util to get github token for user from git config
    '''
    res = subprocess.Popen(["git", "config", "user.apitoken"], stdout=subprocess.PIPE).communicate()[0]
    return res[:-1].decode()


def get_assets_json(name):
    with open(name) as f:
        j = json.load(f)
    return j


def verify_config(j):
    '''
    checks for required fields for json description file
    '''
    required = ['assets']
    for val in required:
        if val not in j:
            raise(RuntimeError(f"value '{val}' not found in config!"))


class GitOps:
    '''
    Simplest github download wrapper based on ghapi (passed by api)
    please see reference here: https://ghapi.fast.ai/fullapi.html
    '''
    def __init__(self, api: GhApi, cache_dir: str, install_dir, j: dict):
        self.api = api
        self.j = j
        self.cache = Path(cache_dir)
        self.cache.expanduser().mkdir(exist_ok=True)
        self.install_dir = install_dir

    def get_cached(self, file_name, git_sha):
        '''
        create catalog in downloads for file
            file_name - file to download as
            git_sha - git sha, `ref` by ghapi v3, if sha is not provided `master` is taken
        '''
        where = (self.cache / git_sha / file_name).expanduser()
        if not where.parent.exists():
            where.parent.mkdir(parents=True)
        return where

    def create_download_data(self, val, ref):
        '''
        generate 'data' required for github api to download via get_content
        essentially:
            val - file to take, remove `./` if required - http paths doesn't have ./
            ref - git sha to use for download
        '''
        file_name = val['name']
        if './' == file_name[0:2]:
            file_name = file_name[2:]
        data = {"path": file_name, "ref": ref}
        return data

    def download_file_from_git(self, where, data):
        '''
        use ghapi to download
            where - where to download file to (please remember to download to cache)
            data - what to download
        '''
        path = self.api.repos.get_content(**data)
        with where.open("wb") as f:
            # TODO in generall - content says what encoding is used, this is simplification
            data = b64decode(path["content"])
            f.write(data)

    def copy_file(self, what: Path, where: Path):
        '''
        if there is no path to `where` create -> then copy `what`
        '''
        import shutil
        where.parent.mkdir(exist_ok=True, parents=True)
        log.debug(f'{what} -> {where}')
        shutil.copy(what, where)

    @lru_cache(maxsize=None)
    def fallback_ref(self):
        '''
        get master sha, cached as asking api visible time and there is no need to do so
        '''
        for val in self.api.repos.list_branches():
            if val['name'] == 'master':
                log.debug(f"using {val['commit']['sha']} for master")
                return val['commit']['sha']
        raise RuntimeError('Master not found!')

    def download_json(self):
        '''
        download function which uses our json to download all files required
        -> check if file for sha exists -> if not: download -> copy where needed
        '''
        for idx, val in enumerate(tqdm(self.j['assets'])):
            data = None
            if 'name' not in val:
                raise RuntimeError(f'there is no name in json->assets on position {idx}')
            try:
                git_sha = val['ref'] if 'ref' in val else self.fallback_ref()
                data = self.create_download_data(val, git_sha)
                cached = self.get_cached(data['path'], git_sha)
                if not cached.exists():
                    log.debug(f"downloading: {data} to: {str(cached)}")
                    self.download_file_from_git(cached, data)
                output = Path(val['output']) if 'output' in val else Path(val['name'])
                if output.is_absolute() and self.install_dir is not None:
                    raise RuntimeError("cant have absolute output with install dir...")
                if not output.is_absolute() and self.install_dir is not None:
                    log.debug("prepend install dir")
                    output = Path(self.install_dir) / output
                output.parent.mkdir(parents=True, exist_ok=True)
                self.copy_file(cached, output)
            except HTTP404NotFoundError as ex:
                raise RuntimeError(f'file not found with: {data} err: {ex}')
            except HTTP403ForbiddenError as ex:
                # gh is messed up - if you get persistent error on this file, try renaming
                raise RuntimeError(f'something is wrong with: {data} err: {ex}')


def arguments():
    import argparse
    parser = argparse.ArgumentParser(description="download assets from repo, requires valid token in git config")
    parser.add_argument('--json', help="json file with description what shall we load", required=True)
    subparsers = parser.add_subparsers(title='cmd', description="command to run", required=True, dest='cmd')
    git_args = subparsers.add_parser('github', description="download assets from github")
    git_args.add_argument('--owner', help="owner to take data from, in our case Mudita", default="mudita")
    git_args.add_argument('--repository', help='repository to take assets from', default='MuditaOSAssets')
    git_args.add_argument('--cache_dir', help='cache dir to store downloaded files', default='~/.mudita/')
    git_args.add_argument('--install_dir', help='optional install dir for path, default - relative to script', default=None)
    git_args = subparsers.add_parser('local', description="just copy assets from local directory")
    return parser.parse_args()


if __name__ == "__main__":
    try:
        log.info('download_assets')
        args = arguments()
        if args.cmd == 'github':
            log.info('get token...')
            token = getToken()
            log.info('get config...')
            j = get_assets_json(args.json)
            log.info('verify config...')
            verify_config(j)
            log.info('use GhApi...')
            api = GhApi(owner=args.owner, token=token, repo=args.repository)
            downloader = GitOps(api, args.cache_dir, args.install_dir, j)
            downloader.download_json()
            log.info('downloader success')
        if args.cmd == 'local':
            raise RuntimeError('Not implemented')
    except RuntimeError as ex:
        log.error(ex)
        log.error('downloader exit with error!')
        exit(1)
