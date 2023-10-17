// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include <blkid.h>
#include "parse_partitions.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <fcntl.h>
#include <unistd.h>
static const size_t sector_size      = 512;
static const size_t bootstrap_offset = 0x00E0;

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
    if (!ls) {
        *nelems = 0;
        return pret;
    }
    const int nparts = blkid_partlist_numof_partitions(ls);
    if (nparts < 1) {
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
            pret[ipart].end   = (blkid_partition_get_start(par) + blkid_partition_get_size(par) - 1LLU) * sector_size;
            pret[ipart].type  = blkid_partition_get_type(par);
            ++ipart;
        }
        *nelems = ipart;
    }
    blkid_free_probe(pr);
    return pret;
}

void print_partitions(const struct partition *part, size_t nparts)
{
    printf("List of partitions [%lu]:\n", nparts);
    if (!part) {
        return;
    }
    for (size_t s = 0; s < nparts; ++s) {
        printf("    Number: [%lu] Type: [%02x] Start: [%luk] End: [%luk]\n",
               s + 1,
               (int)part->type,
               part[s].start / 1024,
               part[s].end / 1024);
    }
}

static inline unsigned calculate_shift(uint32_t v)
{

    static const int mult_bruijn_bit_position[32] = {0, 9,  1,  10, 13, 21, 2,  29, 11, 14, 16, 18, 22, 25, 3, 30,
                                                     8, 12, 20, 28, 15, 17, 24, 7,  19, 27, 23, 6,  26, 5,  4, 31};

    v |= v >> 1; // first round down to one less than a power of 2
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    return mult_bruijn_bit_position[(uint32_t)(v * 0x07C4ACDDU) >> 27];
}

static ssize_t sector_block_size(int filedes)
{
    struct stat statbuf;
    int err = fstat(filedes, &statbuf);
    if (err < 0) {
        return err;
    }
    uint64_t blk_sz = 0;
    if (S_ISBLK(statbuf.st_mode)) {
        err = ioctl(filedes, BLKSSZGET, &blk_sz);
        if (err < 0) {
            return err;
        }
    }
    else {
        blk_sz = 512;
    }
    return blk_sz;
}

int write_partition_bootunit(const char *filename, int part_num, uint32_t block_size)
{
    if (!filename) {
        errno = EINVAL;
        return -1;
    }
    const int fd = open(filename, O_RDWR);
    if (fd < 0) {
        return -1;
    }
    const ssize_t sector_size = sector_block_size(fd);
    if (sector_size <= 0) {
        return -1;
    }
    char *const sect_buf = malloc(sector_size);
    if (!sect_buf) {
        return -1;
    }
    int ret;
    if ((ret = read(fd, sect_buf, sector_size)) != sector_size) {
        close(fd);
        free(sect_buf);
        return -1;
    }
    const uint8_t log2_block_size         = calculate_shift(block_size);
    sect_buf[bootstrap_offset + part_num] = log2_block_size;
    if (lseek(fd, 0, SEEK_SET) < 0) {
        close(fd);
        free(sect_buf);
        return -1;
    }
    if (write(fd, sect_buf, sector_size) != sector_size) {
        close(fd);
        free(sect_buf);
        return -1;
    }
    free(sect_buf);
    return close(fd);
}
