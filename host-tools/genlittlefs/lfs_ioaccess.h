// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

struct lfs_config;
struct partition;

int lfs_ioaccess_open(struct lfs_config *cfg, const char *filename, const struct partition *partition);

int lfs_ioaccess_close();
