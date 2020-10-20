#!/bin/bash
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

echo "Building all submodules while checking out from MASTER branch."

git submodule update 
git submodule foreach git checkout master --recursive
git submodule foreach git pull origin master --recursive