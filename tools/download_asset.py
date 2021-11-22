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
from os import environ
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
log.setLevel(logging.INFO)


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
    - try env first
    - then try gitconfig
    '''
    apitoken = environ.get('GITHUB_TOKEN')
    if apitoken is not None:
        return apitoken
    res = subprocess.Popen(["git", "config", "user.apitoken"], stdout=subprocess.PIPE).communicate()[0]
    apitoken = res[:-1].decode()
    return apitoken


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
        self.install_dir = Path(install_dir if install_dir is not None else '.')

    def get_cached(self, file_name, any_id):
        '''
        create catalog in downloads for file
            file_name - file to download as
            any_id - ideal git sha, `ref` by ghapi v3, if sha is not provided `master` is taken for repo access, id for release
        '''
        where = (self.cache / any_id / file_name).expanduser()
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

    def download_file_from_release(self, id, name_out):
        '''
        This function:
            - gets file data via github api v3
            - then downloads it from the same link with forward by header:
                'application/octet-stream'
        '''
        cached = self.get_cached(name_out, str(id))
        where = self.install_dir / name_out
        if not cached.exists():
            oldheaders = self.api.headers['Accept']
            log.info(f"downloading {cached.absolute()}")
            data = self.api.repos.get_release_asset(id)
            # set headers to accept octet stream instead of ghapi
            self.api.headers['Accept'] = 'application/octet-stream'
            import requests
            response = requests.get(data['url'],
                                    headers={**self.api.headers},
                                    stream=True)
            if response.status_code != requests.codes.ok:
                raise RuntimeError(f"download error: {response.status_code} {response.content}")
            with cached.open('wb') as fd:
                with tqdm(total=data['size'], unit='kb', unit_scale=True) as p_bar:
                    for chunk in response.iter_content(chunk_size=1024):
                        fd.write(chunk)
                        p_bar.update(len(chunk))
            # restore headers
            self.api.headers['Accept'] = oldheaders
        self.copy_file(cached, where)

    def copy_file(self, what: Path, where: Path):
        '''
        if there is no path to `where` create -> then copy `what`
        '''
        import shutil
        where.parent.mkdir(exist_ok=True, parents=True)
        log.info(f'{what} -> {where}')
        shutil.copy(what, where)

    @lru_cache(maxsize=None)
    def _api_list_branches(self):
        return self.api.repos.list_branches()

    @lru_cache(maxsize=None)
    def _api_list_releases(self):
        return self.api.repos.list_releases()

    def fallback_ref(self):
        '''
        get master sha, cached as asking api visible time and there is no need to do so
        '''
        for val in self._api_list_branches():
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
                    log.info(f"downloading: {data} to: {str(cached)}")
                    self.download_file_from_git(cached, data)
                output = Path(val['output']) if 'output' in val else Path(val['name'])
                if output.is_absolute() and self.install_dir is not None:
                    raise RuntimeError("cant have absolute output with install dir...")
                if not output.is_absolute() and self.install_dir is not None:
                    log.debug("prepend install dir")
                    output = Path(self.install_dir) / output
                output.parent.mkdir(parents=True, exist_ok=True)
                if 'unpack' in val:
                    import tarfile
                    with tarfile.open(cached) as tar:
                        tar.extract(output.name, path=output.parent)
                        log.info(f'{cached}[{output.name}] -[untar]-> {output.parent / output.name}')
                else:
                    self.copy_file(cached, output)
            except HTTP404NotFoundError as ex:
                raise RuntimeError(f'file not found with: {data} err: {ex}')
            except HTTP403ForbiddenError as ex:
                # gh is messed up - if you get persistent error on this file, try renaming
                raise RuntimeError(f'something is wrong with: {data} err: {ex}')

    def _get_product_by_name(self, repository: str, product: str, version: str) -> str:
        '''
        We have naming convetion of releases:
        "$Product $Version"
        where:
            - product is product built i.e. ecoboot
            - version - is semantic version we use
        '''
        if "bell" in product.lower():
            product = "bell"
        elif "pure" in product.lower():
            product = "pure"
        else:
            log.error("we require short version in this script - either pure or bell")
            sys.exit(2)

        return f"{repository} {version}-{product}"


    def download_release(self, repo, name_in, name_out, version, product):
        release_requested = self._get_product_by_name(repo, product, version)
        release = None
        for rel in self._api_list_releases():
            # warning just taken first from partial match, should be done differently
            if release_requested in rel['name']:
                release = rel
                break
        if release is None:
            raise RuntimeError(f"Can't get release by release name: {release_requested} with args: {args}")
        log.info(f"Trying to download release from {args.repository} under name: {release_requested}")
        assets = release['assets']
        found = False
        for asset in assets:
            if asset['name'] == name_in:
                self.download_file_from_release(asset['id'], name_out)
                found = True
        if not found:
            raise(RuntimeError(f"asset not found {args.assetRepoName} and cant be saved as {args.assetOutName}"))

    def list_releases(self):
        '''
        list releases from repository selected in pretty format
        '''
        from tabulate import tabulate
        headers = ["tag", "name", "date", "pre-release"]
        table = [[r['tag_name'], r['name'], r['published_at'], r['prerelease']] for r in self._api_list_releases()]
        # print is used "by design" here - deliberately not using `logging` here
        print(tabulate(table, headers, tablefmt="github"))


def arguments():
    import argparse
    parser = argparse.ArgumentParser(description="download assets from repo, requires valid token in git config")
    subparsers = parser.add_subparsers(title='cmd', description="command to run", required=True, dest='cmd')
    cmd_git = subparsers.add_parser('github', description="download assets from github")
    cmd_git.add_argument('--owner', help="owner to take data from, in our case Mudita", default="mudita")
    cmd_git.add_argument('--repository', help='repository to take assets from', default='MuditaOSAssets')
    cmd_git.add_argument('--cache_dir', help='cache dir to store downloaded files', default='~/.mudita/')
    cmd_git.add_argument('--install_dir', help='optional install dir for path, default - relative to script', default=None)
    subparsers_git = cmd_git.add_subparsers(title='github cmd', description="util to run with github", dest='cmd_github')
    cmd_git_json = subparsers_git.add_parser('json', description="download assets by json file")
    cmd_git_json.add_argument('--json', help="json file with description what shall we load", required=True)
    cmd_git_list = subparsers_git.add_parser('list', description="list releases from github")
    cmd_git_release_download = subparsers_git.add_parser('download', description="download assets from github release")
    cmd_git_release_download.add_argument('--name_in', required=True, help="The name of asset to download from release package in repository")
    cmd_git_release_download.add_argument('--name_out', required=True, help="The output name to assign to downloaded asset after download")
    cmd_git_release_download.add_argument('--version', required=True, help="Asset version to download")
    cmd_git_release_download.add_argument('--product', required=True, help='Asset product to download, has to match any product we support at the moment')


    # cmd_git_list =
    subparsers.add_parser('local', description="local files copy")
    return parser.parse_args()


if __name__ == "__main__":
    try:
        args = arguments()
        log.info('download_assets.py')
        if args.cmd == 'github':
            log.debug('get token...')
            token = getToken()
            if args.cmd_github == 'json':
                log.debug('get config...')
                j = get_assets_json(args.json)
                log.debug('verify config...')
                verify_config(j)
                api = GhApi(owner=args.owner, token=token, repo=args.repository)
                downloader = GitOps(api, args.cache_dir, args.install_dir, j)
                downloader.download_json()
                log.info('downloader success')
            if args.cmd_github == 'list':
                api = GhApi(owner=args.owner, token=token, repo=args.repository)
                downloader = GitOps(api, args.cache_dir, args.install_dir, {})
                downloader.list_releases()
                log.info('downloader success')
            if args.cmd_github == 'download':
                if args.install_dir is None:
                    args.install_dir = Path('./')
                api = GhApi(owner=args.owner, token=token, repo=args.repository)
                downloader = GitOps(api, args.cache_dir, args.install_dir, {})
                downloader.download_release(args.repository, args.name_in, args.name_out, args.version, args.product)
                log.info('downloader success')
        elif args.cmd == 'local':
            raise RuntimeError('Not implemented')
        else:
            raise RuntimeError('Not implemented')
    except RuntimeError as ex:
        log.error(ex)
        log.error('downloader exit with error!')
        exit(1)
