#!/usr/bin/env python3
'''
Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
'''

from git.repo import Repo
from os import getcwd
from jira import JIRA
import os
import re
import sys


release_base_regex = re.compile(r'^release-(\d+).(\d+).(\d+)$')
rc_regex = re.compile(release_base_regex.pattern[:-1] + r'-rc(\d+)$')
release_regex = re.compile(release_base_regex.pattern[:-1] + r'(-internal)?$')
commit_regex = re.compile(r'^\[((EGD|BH|CP)-\d+)\] .*$')


class Version(object):
    def __init__(self, major, minor, subminor, internal=False):
        self.major = major
        self.minor = minor
        self.subminor = subminor
        self.internal = internal

    @staticmethod
    def from_tag(tag):
        match = release_regex.match(tag)
        if match is None:
            raise RuntimeError("Invalid version tag " + tag)

        internal = match.lastindex == 4 and match.group(4) == "-internal"

        return Version(int(match.group(1)), int(match.group(2)), int(match.group(3)), internal)

    def __str__(self):
        s = "{}.{}.{}".format(self.major, self.minor, self.subminor)
        if self.internal:
            s += "-internal"
        return s

    def __gt__(self, other):
        if self.major == other.major:
            if self.minor == other.minor:
                return self.subminor > other.subminor
            else:
                return self.minor > other.minor
        else:
            return self.major > other.major

    def is_next(self, other):
        if self.major == other.major:
            if self.minor == other.minor:
                return self.subminor - other.subminor == 1
            else:
                return self.minor - other.minor == 1 and self.subminor == 1
        else:
            return self.major - other.major == 1 and self.minor == 0 and self.subminor == 0

    def tag(self):
        return "release-" + str(self)


def jira_connect():
    juser = os.environ.get('JIRA_USER')
    jtoken = os.environ.get('JIRA_TOKEN')
    jserver = "https://appnroll.atlassian.net"
    options = {'server': jserver}
    return JIRA(options, basic_auth=(juser, jtoken))


def main(args):
    if len(args) == 2:
        repo = Repo(args[1])
    else:
        repo = Repo(getcwd())

    # get all versions
    versions = [Version.from_tag(str(t))
                for t in repo.tags if release_regex.match(str(t))]
    versions.sort()
    current_version = versions[-1]

    # make new version looking for the last RC version
    rc = [t for t in repo.tags if rc_regex.match(str(t))][-1]
    match = rc_regex.match(str(rc))
    if match is None:
        print("No RC version")
        return 1
    new_version = Version(int(match.group(1)), int(
        match.group(2)), int(match.group(3)))

    print("Current version : " + str(current_version))
    print("Next version    : " + str(new_version))

    commits = [c for c in repo.iter_commits(
        rev=current_version.tag() + ".." + str(rc))]

    issues = []
    for c in commits:
        match = commit_regex.match(c.summary)
        if match is None:
            print("Skipping commit: " + c.summary)
            continue
        issue = match.group(1)
        if issue in issues:
            print("Skipping duplicate issue: " + issue)
            continue

        issues.append(issue)

    issues.sort()

    # jira
    jira = jira_connect()
    egd = jira.project('EGD')
    sversions = [str(v) for v in jira.project_versions(egd)]
    if str(new_version) in sversions:
        print("JIRA version found: " + str(new_version))
    else:
        print("Creating JIRA version: " + str(new_version))
        jira.create_version(str(new_version), egd)

    for issueName in issues:
        print("Updating version for: " + issueName)
        try:
            issue = jira.issue(issueName)
        except:
            print("Failed to fetch version")
            continue
        fix_versions = [str(v) for v in issue.fields.fixVersions]
        if str(new_version) in fix_versions:
            print(str(issue) + " alredy in version " + str(new_version))
            continue

        update_versions = []
        for v in fix_versions:
            update_versions.append({'name': v})
        update_versions.append({'name': str(new_version)})
        try:
            issue.update(fields={'fixVersions': update_versions})
        except:
            print("Failed to update issue")

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
