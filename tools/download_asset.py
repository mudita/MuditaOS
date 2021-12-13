#!/usr/bin/python3
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
            "ref": "fd168040c5d1216d457e6cf223e8ea9bb76bf7b",       <-- from what ref shall we download the file ignored for local files
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
from dataclasses import dataclass
from typing import Union, Callable, Any, Generator, IO
import logging
from typing import List  # for ancient python3 support


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
    if type(j) is not dict:
        RuntimeError(f"config not in dict form - {type(j)}")
    required = ['assets']
    for val in required:
        if val not in j:
            raise RuntimeError(f"value '{val}' not found in config! {j}")


@dataclass
class Verse:
    '''
    json file dict field in use here

    Args:
        name of file remotely
        tarfile - optional
        output path to file where wa want to have it
        ref reference of file, used as cache version
        unpack information if we shall unpack the file to get output
        sha_gen generator for sha if is missing just in case
    '''
    name: str
    output: Path = Path()
    tarfile: str = ""
    ref: str = ""
    unpack: bool = False
    copy: bool = False
    sha_gen: Callable[['Verse'], str] = lambda _: "none"

    def __post_init__(self):
        '''
            val - file to take, remove `./` if required - http paths doesn't have ./
            ref - git sha to use for download
        '''
        if './' == self.name[0:2]:
            self.name = self.name[2:]
        if self.ref == "":
            self.ref = self.sha_gen(self)
        if self.output == Path():
            self.output = Path(self.name)
        if self.tarfile != "":
            self.unpack = True
            if './' == self.tarfile[0:2]:
                self.tarfile = self.tarfile[2:]
        if not self.unpack:
            self.copy = True
        elif self.tarfile == "":
            raise RuntimeError(f"Wrong verse! it's unpack target but have no `tarfile` set {self}")


@dataclass
class Release:
    name: str
    id: int
    assets: List[Any]


class FileCache:
    '''
    file cache utility, right now - file cache is in cache dir
    if any improvement was to be made we should just take any ready
    library (i.e. DiskCache)
    '''

    def __init__(self, where: str):
        self.cache = Path(where)
        self.cache.expanduser().mkdir(exist_ok=True)

    def _key_gen(self, name, sha_gen=Callable[[], str]) -> Path:
        '''
        cache key generator to access cached file
        '''
        return (self.cache / sha_gen() / name).expanduser()

    def _get_by_verse(self, verse: Verse) -> Path:
        '''
        get path to cache

        Args
            file_name - file to download as
            any_id - ideal git sha, `ref` by ghapi v3, if sha is not provided `master` is taken for repo access, id for release
        '''
        if verse.name is None or verse.name == '':
            raise RuntimeError(f'verse.name has to be set properly! {verse}')
        where = self._key_gen(verse.name, lambda: verse.ref)
        existed = where.parent.exists()
        if not existed:
            where.parent.mkdir(parents=True)
        return where

    def _get_by_gen(self, what, sha_gen: Callable[[], str]) -> Path:
        if what is None or what == '':
            raise RuntimeError(f'input filename has to be set properly! none file passed! {what}')
        return self._key_gen(what, sha_gen)

    def get(self, *args) -> Path:
        ret = None
        if len(args) == 1:
            ret = self._get_by_verse(*args)
        else:
            ret = self._get_by_gen(*args)
        if ret is None:
            raise RuntimeError("cache get failure")
        return ret

    def open(self, what: str, sha_gen: Callable[[], str], mode='rwb') -> Generator[IO, None, None]:
        '''
        return opened cache file
        '''
        with self.get(what, sha_gen).open(mode) as f:
            yield f


class BaseOps:
    def __init__(self, cache_dir, install_dir: Union[str, None]):
        self.cache = FileCache(cache_dir)

        dir = Path(install_dir) if install_dir is not None else Path('./')
        if dir.exists():
            self.install_dir = dir
        else:
            log.warning(f"install dir: {dir} doesn't exist - using {Path('./')} as install dir instead")
            self.install_dir = Path('./')

    def install(self, val: Verse):
        cached = self.cache.get(val)
        # check if file in cache - if not, then populate
        if not cached:
            log.error('no file cached - use {val.name} as {cached}')
            self.copy_file(Path(val.name), cached)

        # get path to install the file
        if Path(val.output).parent.is_absolute():
            raise RuntimeError('do not use absolute paths, use install dir instead')
        install_path = self.install_dir / val.output

        # select proper installation method
        if val.unpack:
            self.unpack(cached, val.tarfile, install_path, val.ref)
        elif val.copy:
            self.copy_file(cached, install_path)
        else:
            raise RuntimeError('no option selected for install, required either unpack or copy')

    def copy_file(self, what: Path, where: Path) -> None:
        log.debug(f'{what} -> {where}')
        self._copy_file(what, where)

    def _copy_file(self, what: Path, where: Path) -> None:
        '''
        if there is no path to `where` create -> then copy `what`
        '''
        import shutil
        self._assert_parent_dir(where)
        shutil.copy(what, where)

    def unpack(self, file_name: Path, what: str, where: Path, id) -> None:
        import tarfile
        self._assert_parent_dir(where)
        cached = self.cache.get(what, lambda: id)
        in_cache = cached.exists()
        if not in_cache:
            try:
                with tarfile.open(file_name) as tar:
                    tar.extract(what, path=cached.parent)
                    (cached.parent / what).rename(cached)
            except KeyError:
                raise RuntimeError(f"file {what} not found in tar: {file_name}")
            except tarfile.ReadError:
                raise RuntimeError(f"cant read tar: {file_name}, cant unpack: {what}")
        self._copy_file(cached, where)
        log.debug(f'{"cached" if cached else "unpack"}: {file_name}[{what}] -> {where}')

    def _assert_parent_dir(self, where: Path):
        if not where.parent.exists():
            log.debug(f"Output file catalog doesn't exist: {str(where.parent)} creating...")
            where.parent.mkdir(parents=True)


class GitOps(BaseOps):
    '''
    Simplest github download wrapper based on ghapi (passed by api)
    please see reference here: https://ghapi.fast.ai/fullapi.html
    '''
    def __init__(self, api: GhApi, cache_dir: str, install_dir, j: dict):
        BaseOps.__init__(self, cache_dir, install_dir)
        self.api = api
        self.j = j
        self.install_dir = Path(install_dir if install_dir is not None else '.')

    def _download_file_from_git(self, where, data):
        '''
        use ghapi to download file straight from git
            where - where to download file to (please remember to download to cache)
            data - what to download
        '''
        try:
            path = self.api.repos.get_content(**data)
        except HTTP404NotFoundError:
            raise RuntimeError(f'file {data["path"]} under id {data["ref"]} not found')
        with where.open("wb") as f:
            # NOTE in general - content says what encoding is used, this is a simplification
            data = b64decode(path["content"])
            f.write(data)

    def _download_file_from_release(self, val):
        '''
        use ghapi to download file straight from gh release
        there is hack to change headers to accept octet-stream : otherwise we wont get file, but it's metadata instead
        see `self.api.headers['Accept']
            val    - verse to download ref is `id` for request
        '''
        where = self.cache.get(val)
        log.info(f"downloading {val.name} to cache: {where}")
        oldheaders = self.api.headers['Accept']
        # get data for file to 
        data = self.api.repos.get_release_asset(val.ref)
        self.api.headers['Accept'] = 'application/octet-stream'
        import requests
        response = requests.get(data['url'],
                                headers={**self.api.headers},
                                stream=True)
        if response.status_code != requests.codes.ok:
            raise RuntimeError(f"download error: {response.status_code} {response.content}")

        # save file to cache my friend
        with where.open('wb') as fd:
            with tqdm(total=data['size'], unit='kb', unit_scale=True) as p_bar:
                for chunk in response.iter_content(chunk_size=1024):
                    fd.write(chunk)
                    p_bar.update(len(chunk))

        # restore headers
        self.api.headers['Accept'] = oldheaders

    def download_file_from_release(self, val: Verse):
        # cache after the release id not after the file id - to not download the release multiple times
        cached = self.cache.get(val)
        if not cached.exists():
            self._download_file_from_release(val)
        self.install(val)

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
            try:
                val = Verse(**val, sha_gen=lambda _: self.fallback_ref())
                cached = self.cache.get(val)
                if not cached.exists():
                    data = {"path": val.name, "ref": val.ref}
                    log.info(f"downloading: {data} to: {str(cached)}")
                    self._download_file_from_git(cached, data)
                output = Path(val.output)
                if output.is_absolute() and self.install_dir is not None:
                    raise RuntimeError("cant have absolute output with install dir...")
                self.install(val)
            except TypeError as ex:
                log.error(ex)
                raise RuntimeError(f'there is no name in json->assets on position {idx}')
            except HTTP404NotFoundError as ex:
                raise RuntimeError(f'file not found with: {data} err: {ex}')
            except HTTP403ForbiddenError as ex:
                # gh is messed up - if you get persistent error on this file, try renaming
                raise RuntimeError(f'something is wrong with: {data} err: {ex}')

    @lru_cache(maxsize=None)
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
            log.error(f"we require short version in this script - either pure or bell, we got: repository: {repository} product: {product} version: {version}")
            exit(1)

        return f"{repository} {version}-{product}"

    def _get_lousy_match_release(self, repo, product, version) -> Release:
        '''
        releases have to be named in format: `RepoName product version`
        '''
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

        return Release(release['id'], release['name'], release['assets'])

    def _get_file_in_release(self, assets, name: str):
        for asset in assets:
            if asset['name'] == name:
                return str(asset['id'])
        raise RuntimeError(f"asset not found {name} not found in {assets}")

    def download_release_file(self, repo, name_in, name_out, version, product):
        '''
        Downloads file named `name_in` from release matching `version` and `product`
        then installs it under `name_out`
        '''
        release = self._get_lousy_match_release(repo, product, version)
        var = Verse(name_in, name_out, sha_gen=lambda x: self._get_file_in_release(release.assets, x.name))
        self.download_file_from_release(var)

    def download_release_json(self, repo, j, version, product):
        '''
        Downloads file to cache if not exists then copy where needed
        '''
        release = self._get_lousy_match_release(repo, product, version)
        for el in [Verse(**val, sha_gen=lambda x: self._get_file_in_release(release.assets, x.name)) for val in j['assets']]:
            self.download_file_from_release(el)

    def list_releases(self):
        '''
        list releases from repository selected in pretty format
        '''
        from tabulate import tabulate
        headers = ["tag", "name", "date", "pre-release"]
        table = [[r['tag_name'], r['name'], r['published_at'], r['prerelease']] for r in self._api_list_releases()]
        # print is used "by design" here - deliberately not using `logging` here
        print(tabulate(table, headers, tablefmt="github"))


class LocalOps(BaseOps):
    '''
    quick and dirty local copy class
    '''
    def __init__(self, install_dir, cache_dir):
        BaseOps.__init__(self, cache_dir, install_dir)

    def copy_files(self, j={}):
        if 'assets' not in j:
            raise RuntimeError("json description file has to have assets dict, see download_asset.py module docs")
        for val in [Verse(**val) for val in j['assets']]:
            if not val.copy:
                raise RuntimeError("assets row for copy has to have copy marker")
            val.output = val.output if val.output.is_absolute() else self.install_dir / val.output
            self.install(val)


def arguments():
    import argparse
    parser = argparse.ArgumentParser(description="download assets from repo, requires valid token in git config")
    parser.add_argument('--cache_dir', help='cache dir to store downloaded files', default='~/.mudita/')
    subparsers = parser.add_subparsers(title='cmd', description="command to run", required=True, dest='cmd')
    cmd_git = subparsers.add_parser('github', description="download assets from github")
    cmd_git.add_argument('--owner', help="owner to take data from, in our case Mudita", default="mudita")
    cmd_git.add_argument('--repository', help='repository to take assets from', default='MuditaOSAssets')
    cmd_git.add_argument('--install_dir', help='optional install dir for path, default - relative to script', default=None)

    subparsers_git = cmd_git.add_subparsers(title='github cmd', description="util to run with github", dest='cmd_github')
    cmd_git_json = subparsers_git.add_parser('json', description="download assets by json file")

    cmd_git_json.add_argument('--json', help="json file with description what shall we load", required=True)

    cmd_git_list = subparsers_git.add_parser('list', description="list releases from github")

    cmd_git_release_download = subparsers_git.add_parser('download', description="download assets from github release")

    cmd_git_release_download.add_argument('--json', help="json file with description what shall we load")
    cmd_git_release_download.add_argument('--name_in',help="The name of asset to download from release package in repository")
    cmd_git_release_download.add_argument('--name_out', help="The output name to assign to downloaded asset after download")

    cmd_git_release_download.add_argument('--version', required=True, help="Asset version to download")
    cmd_git_release_download.add_argument('--product', required=True, help='Asset product to download, has to match any product we support at the moment')

    local_parser = subparsers.add_parser('local', description="local files copy")
    local_parser.add_argument('--json', help="json file with description what shall we load", required=True)
    local_parser.add_argument('--install_dir', help='optional install dir for path, default - relative to script', default=None)
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
            if args.cmd_github == 'list':
                api = GhApi(owner=args.owner, token=token, repo=args.repository)
                downloader = GitOps(api, args.cache_dir, args.install_dir, {})
                downloader.list_releases()
            if args.cmd_github == 'download':
                has_json = args.json is not None
                has_names = args.name_in is not None and args.name_out is not None
                if (not has_json and not has_names) or (has_json and has_names):
                    raise RuntimeError(f'script requires either json or name_in and name_out parameters to download from release {"have json file selected" if has_json else ""} {"have names selected" if has_names else ""}')
                api = GhApi(owner=args.owner, token=token, repo=args.repository)
                downloader = GitOps(api, args.cache_dir, args.install_dir, {})
                if has_json:
                    log.debug('download release from json')
                    j = get_assets_json(args.json)
                    log.debug('verify config...')
                    verify_config(j)
                    downloader.download_release_json(args.repository, j, args.version, args.product)
                else:
                    log.debug('download release file')
                    downloader.download_release_file(args.repository, args.name_in, args.name_out, args.version, args.product)
        elif args.cmd == 'local':
            j = get_assets_json(args.json)
            log.debug('verify config...')
            verify_config(j)
            downloader = LocalOps(args.install_dir, args.cache_dir)
            downloader.copy_files(j)
        else:
            raise RuntimeError('Not implemented')
    except RuntimeError as ex:
        log.error(ex)
        log.error('downloader exit with error!')
        exit(1)
