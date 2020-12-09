// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <blkid.h>
#include "parse_partitions.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static const size_t sector_size = 512;

struct partition *find_partitions(const char *filename, part_type_t ptype, size_t *nelems)
{
    struct partition *pret = NULL;
    blkid_probe pr;
    pr = blkid_new_probe_from_filename(filename);
    if (!pr) {
        return pret;
    }
    blkid_probe_enable_partitions(pr, true);
    blkid_do_fullprobe(pr);

    blkid_partlist ls = blkid_probe_get_partitions(pr);
    const int nparts  = blkid_partlist_numof_partitions(ls);
    if (nparts < 1) {
        if (nelems)
            *nelems = 0;
        return pret;
    }
    else {
        pret = calloc(nparts, sizeof(struct partition));
    }
    size_t ipart = 0;
    for (int i = 0; i < nparts; ++i) {
        blkid_partition par = blkid_partlist_get_partition(ls, i);
        if (ptype == scan_all_partitions || blkid_partition_get_type(par) == ptype) {
            pret[ipart].start = (blkid_partition_get_start(par)) * sector_size;
            pret[ipart].end   = pret[i].start + blkid_partition_get_size(par) * sector_size;
            pret[ipart].type  = blkid_partition_get_type(par);
            ++ipart;
        }
        if (nelems) {
            *nelems = ipart;
        }
    }
    blkid_free_probe(pr);
    return pret;
}
