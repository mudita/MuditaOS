#!/usr/bin/env python3
'''
Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
'''

import os
from git import Repo
import re


def get_pull_request_commits():
  github_base_ref = os.environ['GITHUB_BASE_REF']
  github_head_ref = os.environ['GITHUB_HEAD_REF']

  return Repo('.').iter_commits(rev=f'origin/{github_base_ref}..origin/{github_head_ref}')


def validate_commit(commit):
  errors = []

  lines = commit.message.split('\n')

  if len(lines) < 3:
    errors.append(f'[{commit.hexsha}] commit message should be composed of at least 3 lines: a subject, an empty line and a body')
    return errors

  line_length_limit = 72
  if any(len(line) > line_length_limit for line in lines):
    errors.append(f'[{commit.hexsha}] maximum allowed line length is {line_length_limit}')

  subject = lines[0]
  empty_line = lines[1]
  body = ''.join(lines[2:]).strip()

  subject_format = r'^(\[(EGD|BH|CP)-\d+\])+ [A-Z].+[^.]$'
  if not re.match(subject_format, subject):
    errors.append(f'[{commit.hexsha}] invalid subject "{subject}", should match format "{subject_format}"')

  if empty_line:
    errors.append(f'[{commit.hexsha}] the second line in a commit message should be empty')

  if not body:
    errors.append(f"[{commit.hexsha}] message body can't be empty")

  return errors


def main():
  errors = []

  for commit in get_pull_request_commits():
    errors.extend(validate_commit(commit))

  for error in errors:
    print(error)

  if errors:
    print('See https://github.com/mudita/MuditaOS/blob/master/doc/development_workflow.md#commit-changes for the commit message format specification')
    exit(1)


if __name__ == '__main__':
  main()
