// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

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

struct partition *find_partitions(const char *filename, part_type_t ptype, size_t *nelems)
    __attribute__((nonnull(1, 3)));

void print_partitions(const struct partition *part, size_t nparts);

int write_partition_bootunit(const char *filename, int part_num, uint32_t block_size);
