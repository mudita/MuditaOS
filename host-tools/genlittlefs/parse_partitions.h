// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <stddef.h>

typedef int part_type_t;

enum part_type_options
{
    scan_all_partitions = -1
};

struct partition
{
    off_t start;
    off_t end;
    part_type_t type;
};

struct partition *find_partitions(const char *filename, part_type_t ptype, size_t *nelems);