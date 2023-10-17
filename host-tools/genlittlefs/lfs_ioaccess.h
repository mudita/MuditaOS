// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

struct lfs_config;
struct partition;

struct lfs_ioaccess_context;

struct lfs_ioaccess_context *lfs_ioaccess_open(struct lfs_config *cfg,
                                               const char *filename,
                                               const struct partition *partition);

int lfs_ioaccess_close(struct lfs_ioaccess_context *ctx);

int lfs_ioaccess_is_lfs_filesystem(struct lfs_ioaccess_context *ctx);
