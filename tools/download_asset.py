#!/usr/bin/env python3
"""
Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

Download any file from releases from Mudita repository

@package globalFinder
"""

import argparse
import git
import json
import os
import requests
import sys

from tqdm import tqdm
import time


class Getter(object):
    ''' Class to list and download assets in release '''

    def __init__(self):
        self.host = 'https://api.github.com/repos'
        self.organisation = 'mudita'
        self.repo = ''
        self.apitoken = None
        self.ghLogin = None
        self.getGitRoot()
        self.getApiToken()
        self.getGHLogin()
        self.releases = []
        self.workdir = ""

    def genRestPrefix(self):
        self.restPrefix = self.host
        self.restPrefix += '/'
        self.restPrefix += self.organisation
        self.restPrefix += '/'
        if self.repo == '':
            raise ValueError("Repository must be set")
        self.restPrefix += self.repo
        self.restPrefix += '/'
        return self.restPrefix


    def getGitRoot(self):
        'Find git root directory'
        self.gitRepo = git.Repo(os.getcwd(), search_parent_directories=True)
        self.gitRoot = self.gitRepo.git.rev_parse("--show-toplevel")
        return self.gitRoot

    def getApiToken(self, args=None):
        if args is not None:
            if args.token is not None:
                self.apitoken = args.token
                return
        try:
            gitConfigReader = self.gitRepo.config_reader()
            self.apitoken = gitConfigReader.get_value("user", "apitoken")
        except:
            pass

    def getGHLogin(self, args=None):
        if args is not None:
            if args.login is not None:
                self.ghLogin = args.login
                return
        try:
            gitConfigReader = self.gitRepo.config_reader()
            self.ghLogin = gitConfigReader.get_value("user", "githublogin")
        except:
            pass

    def findWorkDir(self):
        '''Finds and sets current workdir (based on 'workdir' argument)'''
        if os.path.isabs(self.args.workdir):
            self.workdir = self.args.workdir
        else:
            self.workdir = self.getGitRoot() + "/" + self.args.workdir

    def getReleases(self, args):
        '''List only releases'''
        print(f"{sys._getframe().f_code.co_name}")
        self.args = args
        self.getGHLogin(args)
        self.getApiToken(args)
        request = self.genRestPrefix() + "releases"
        print (f"Request: {request}")
        headers = {'accept': 'application/vnd.github.v3+json'}
        page = 0
        itemsOnPage = 100
        count = itemsOnPage
        while count == itemsOnPage:
            page += 1
            queryParams = {'per_page': itemsOnPage, 'page': page}
            response = requests.get(request, auth=(self.ghLogin, self.apitoken), headers=headers, params=queryParams)
            if response.status_code != requests.codes.ok:
                print("download error:", response.status_code)
                print("content:", response.content)
                sys.exit(1)
            items = json.loads(response.content)
            count = len(items)
            self.releases += items
        drafts = []
        releases = []
        for item in self.releases:
            if item['published_at'] is None:
                drafts.append(item)
            else:
                releases.append(item)
        releases.sort(key=lambda r: r['published_at'], reverse=True)
        self.releases=drafts
        self.releases.extend(releases)

    def listReleases(self, args):
        self.getReleases(args)
        print(" tag | name | date | pre-release")
        print("--------------------------------")
        for release in self.releases:
            print(release['tag_name'], release['name'], release['published_at'], release['prerelease'], sep=" | ")

    def getProductByName(self, repository: str, product: str, version: str) -> str:
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
            print("we require short version in this script - either pure or bell")
            sys.exit(2)

        return f"{repository} {version}-{product}"

    def downloadRelease(self, args):
        print(f"{sys._getframe().f_code.co_name} from: {args.repository}")
        self.getReleases(args)
        release_requested = self.getProductByName(args.repository, args.product, args.version)
        release = None
        for rel in self.releases:
            # warning just taken first from partial match, should be done differently
            if release_requested in rel['name']:
                release = rel
                break
        if release is None:
            raise RuntimeError(f"Can't get release by release name: {release_requested} with args: {args}")
        print(f"Trying to download release from {args.repository} under name: {release_requested}")
        assets = release['assets']

        found = False
        for asset in assets:
            if asset['name'] == args.assetRepoName:
                self.downloadAsset(asset, args.assetOutName)
                found = True
        if not found:
            raise(RuntimeError(f"asset not found {args.assetRepoName} and cant be saved as {args.assetOutName}"))

    def downloadAsset(self, asset, outName):
        self.createWorkdir()
        print("name:", asset['name'])
        print("url:", asset['url'])
        print("size:", asset['size'])
        headers = {'accept': 'application/octet-stream'}
        response = requests.get(asset['url'],
                                auth=(self.ghLogin, self.apitoken),
                                headers=headers,
                                stream=True)
        destination_file = self.args.workdir + "/" + outName
        print("downloading to:", destination_file)
        progres_bar = tqdm(total=asset['size'], unit='iB', unit_scale=True)
        with open(destination_file, 'wb') as fd:
            for chunk in response.iter_content(chunk_size=1024):
                progres_bar.update(len(chunk))
                fd.write(chunk)
        if response.status_code != requests.codes.ok:
            print("download error:", response.status_code)
            print(response.content)
            sys.exit(2)

    def createWorkdir(self):
        from pathlib import Path
        Path(self.workdir).mkdir(parents=True, exist_ok=True)

    def run(self, args):
        self.args = args
        self.findWorkDir()
        print(self.args)


def main():

    getter = Getter()

    parser = argparse.ArgumentParser(description="Download asset from github release pages")
    parser.add_argument('--workdir', help="Directory where package is build", default="update")
    parser.add_argument('-t', '--token',
                        help="GitHub security token "
                             "by default read from `git config user.apitoken` "
                             "Generate it on https://github.com/settings/tokens "
                             "Set privileges to `repo` "
                             "and store with command: `git config -add user.apitoken <token from GH>",
                        required=(getter.apitoken is None))
    parser.add_argument('-l', '--login',
                        help="GitHub login"
                             "by default read from `git config user.githublogin`"
                             "to add your login use:"
                             "`git config -add user.githublogin <login>`",
                        required=(getter.ghLogin is None))

    parser.add_argument('--repository', help="repository name from which assets will be downloaded")

    parser.set_defaults(func=getter.run)
    subparsers = parser.add_subparsers(title="commands",
                                       description="available commands")
    listReleases_args = subparsers.add_parser('list', aliases=['ll'],
                                              description="List public releases")
    listReleases_args.set_defaults(func=getter.listReleases)

    getReleases_args = subparsers.add_parser('download', aliases=['dw'],
                                             description="Download release based on it's product and version")
    getReleases_args.set_defaults(func=getter.downloadRelease)
    getReleases_args.add_argument('--assetRepoName', help="The name of asset to download from release package in repository", required=True)
    getReleases_args.add_argument('--assetOutName', help="The output name to assign to downloaded asset after download", required=True)
    getReleases_args.add_argument('--version', required=True, help="Asset version to download")
    getReleases_args.add_argument('--product', required=True, help='Asset product to download, has to match any product we support at the moment')

    args = parser.parse_args()
    getter.repo = args.repository
    getter.workdir = args.workdir
    args.func(args)


if __name__ == "__main__":
    main()
