#!/usr/bin/env python3
"""
Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

Download ecooboot.bin from repository

@package globalFinder
"""

import argparse
import git
import json
import os
import requests
import sys
from tqdm import tqdm


class Getter(object):
    '''Download latest ecooboot.bin image'''

    def __init__(self):
        self.host = 'https://api.github.com/repos'
        self.organisation = 'mudita'
        self.repo = 'ecoboot'
        self.restPrefix = self.host
        self.restPrefix += '/'
        self.restPrefix += self.organisation
        self.restPrefix += '/'
        self.restPrefix += self.repo
        self.restPrefix += '/'
        self.apitoken = None
        self.ghLogin = None
        self.getGitRoot()
        self.getApiToken()
        self.getGHLogin()
        self.releases = []
        self.workdir = ""

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
        except git.exc.NoOptionError as error:
            pass

    def getGHLogin(self, args=None):
        if args is not None:
            if args.login is not None:
                self.ghLogin = args.login
                return
        try:
            gitConfigReader = self.gitRepo.config_reader()
            self.ghLogin = gitConfigReader.get_value("user", "githublogin")
        except git.exc.NoOptionError as error:
            pass

    def findWorkDir(self):
        '''Finds and sets current workdir (based on 'workdir' argument)'''
        if os.path.isabs(self.args.workdir):
            self.workdir = self.args.workdir
        else:
            self.workdir = self.getGitRoot() + "/" + self.args.workdir

    def getReleases(self, args):
        '''List only releases'''
        self.args = args
        self.getGHLogin(args)
        self.getApiToken(args)
        request = self.restPrefix + "releases"
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
                print(response.content)
                sys.exit(1)
            items = json.loads(response.content)
            count = len(items)
            self.releases += items
        self.releases.sort(key=lambda r: r['published_at'], reverse=True)

    def listReleases(self, args):
        self.getReleases(args)
        print(" tag | name | date | pre-release")
        print("--------------------------------")
        for release in self.releases:
            print(release['tag_name'], release['name'], release['published_at'], release['prerelease'], sep=" | ")

    def downloadRelease(self, args):
        print(sys._getframe().f_code.co_name)
        self.getReleases(args)
        print(args.tag)
        release = None
        if args.tag is None:
            release = self.releases[0]
        else:
            for rel in self.releases:
                if rel['tag_name'] == args.tag:
                    release = rel
                    break
        if release is None:
            print("No release with tag:", args.tag)
        print("release:", release['tag_name'])
        assets = release['assets']
        self.downloadAsset(assets[0])

    def downloadAsset(self, asset):
        self.createWorkdir()
        print("name:", asset['name'])
        print("url:", asset['url'])
        print("size:", asset['size'])
        headers = {'accept': 'application/octet-stream'}
        response = requests.get(asset['url'],
                                auth=(self.ghLogin, self.apitoken),
                                headers=headers,
                                stream=True)
        progres_bar = tqdm(total=asset['size'], unit='iB', unit_scale=True)
        with open(self.args.workdir + "/" + asset['name'], 'wb') as fd:
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

    parser = argparse.ArgumentParser(description="Download ecooboot")
    parser.add_argument('-w', '--workdir', help="Directory where package is build", default="update")
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

    parser.add_argument('repository', help="repository name from which assets will be downloaded")

    parser.set_defaults(func=getter.run)
    subparsers = parser.add_subparsers(title="commands",
                                       description="available commands")
    listReleases_args = subparsers.add_parser('list', aliases=['ll'],
                                              description="List public releases")
    listReleases_args.set_defaults(func=getter.listReleases)

    getReleases_args = subparsers.add_parser('download', aliases=['dw'],
                                             description="Download Release based on tag or the latest")
    getReleases_args.set_defaults(func=getter.downloadRelease)
    getReleases_args.add_argument('tag', help="Download release with selected tag", nargs='?')

    args = parser.parse_args()
    getter.repo = args.repository
    getter.workdir = args.workdir
    args.func(args)


if __name__ == "__main__":
    main()
