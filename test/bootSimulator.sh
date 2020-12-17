#!/usr/bin/env bash
# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

find ~ -wholename '*linux/PurePhone.elf' -executable -execdir '{}' \; 2> $1
