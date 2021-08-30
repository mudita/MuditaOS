// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <linux/fs.h>
#include <linux/fiemap.h>

static void syntax(char **argv)
{
    fprintf(stderr, "%s filename blkdev\n", argv[0]);
}

static struct fiemap *read_fiemap(int fd)
{
    struct fiemap *fiemap;
    int extents_size;

    if ((fiemap = (struct fiemap *)malloc(sizeof(struct fiemap))) == NULL) {
        fprintf(stderr, "Out of memory allocating fiemap\n");
        return NULL;
    }
    memset(fiemap, 0, sizeof(struct fiemap));

    fiemap->fm_start          = 0;
    fiemap->fm_length         = ~0; /* Lazy */
    fiemap->fm_flags          = 0;
    fiemap->fm_extent_count   = 0;
    fiemap->fm_mapped_extents = 0;

    /* Find out how many extents there are */
    if (ioctl(fd, FS_IOC_FIEMAP, fiemap) < 0) {
        fprintf(stderr, "fiemap ioctl() failed\n");
        return NULL;
    }

    /* Read in the extents */
    extents_size = sizeof(struct fiemap_extent) * (fiemap->fm_mapped_extents);

    /* Resize fiemap to allow us to read in the extents */
    if ((fiemap = (struct fiemap *)realloc(fiemap, sizeof(struct fiemap) + extents_size)) == NULL) {
        fprintf(stderr, "Out of memory allocating fiemap\n");
        return NULL;
    }

    memset(fiemap->fm_extents, 0, extents_size);
    fiemap->fm_extent_count   = fiemap->fm_mapped_extents;
    fiemap->fm_mapped_extents = 0;

    if (ioctl(fd, FS_IOC_FIEMAP, fiemap) < 0) {
        fprintf(stderr, "fiemap ioctl() failed\n");
        return NULL;
    }
    return fiemap;
}

static off_t compare_extent(int fd1, int fd2, off_t start_offs, size_t len)
{
    char buf1[32768], buf2[32768];
    off_t offs;
    ssize_t res1, res2;
    size_t segpos = 0;
    offs          = lseek(fd1, start_offs, SEEK_SET);
    if (offs < 0)
        return offs;
    offs = lseek(fd2, start_offs, SEEK_SET);
    if (offs < 0)
        return offs;
    while (len > 0) {
        const size_t rdsiz = (len > sizeof(buf1)) ? (sizeof buf1) : (len);
        res1               = read(fd1, buf1, rdsiz);
        res2               = read(fd2, buf2, rdsiz);
        if (res1 == res2 && res1 > 0) {
            if (memcmp(buf1, buf2, res1) == 0) {
                len -= res1;
                segpos += res1;
            }
            else {
                return start_offs + segpos + 1;
            }
        }
        else {
            return start_offs + segpos + 1;
        }
    }
    return 0;
}

static ssize_t write_all(int fd, const char *buf, ssize_t size)
{
    ssize_t res;
    while (size > 0 && (res = write(fd, buf, size)) != size) {
        if (res < 0 && errno == EINTR)
            continue;
        else if (res < 0) {
            return res;
        }
        size -= res;
        buf += res;
    }
    return 0;
}

static off_t copy_extent(int fd_dest, int fd_src, off_t start_offs, size_t len)
{
    char copy_buf[32768];
    off_t offs = lseek(fd_dest, start_offs, SEEK_SET);
    if (offs < 0)
        return offs;
    offs = lseek(fd_src, start_offs, SEEK_SET);
    if (offs < 0)
        return offs;
    while (len > 0) {
        const size_t nread = len > sizeof(copy_buf) ? sizeof(copy_buf) : len;
        ssize_t rlen       = read(fd_src, copy_buf, nread);
        if (rlen < 0 && errno == EINTR)
            continue;
        else if (rlen > 0) {
            ssize_t res = write_all(fd_dest, copy_buf, rlen);
            if (res == 0) {
                len -= rlen;
            }
            else {
                return -1;
            }
        }
        else {
            return -1;
        }
    }
    return 0;
}

static int verify_image(const char *image_file, const char *block_device)
{
    int fd_sparse, fd_block;
    if ((fd_sparse = open(image_file, O_RDONLY)) < 0) {
        fprintf(stderr, "Cannot open sparse file %s\n", image_file);
        return EXIT_FAILURE;
    }
    if ((fd_block = open(block_device, O_RDONLY)) < 0) {
        fprintf(stderr, "Cannot open block device %s\n", block_device);
        close(fd_sparse);
        return EXIT_FAILURE;
    }
    struct fiemap *fiemap;
    if (!(fiemap = read_fiemap(fd_sparse))) {
        fprintf(stderr, "Unable to read fiemap %s\n", image_file);
        close(fd_sparse);
        close(fd_block);
        return EXIT_FAILURE;
    }
    printf("File %s verify %d extents:\n", image_file, fiemap->fm_mapped_extents);
    printf("#\tOffset          Length           Verify\n");
    off_t result = -1;
    for (unsigned i = 0; i < fiemap->fm_mapped_extents; i++) {
        result = compare_extent(fd_sparse, fd_block, fiemap->fm_extents[i].fe_logical, fiemap->fm_extents[i].fe_length);
        printf("%d:\t%-16.16llx %-16.16llx ", i, fiemap->fm_extents[i].fe_logical, fiemap->fm_extents[i].fe_length);
        if (result) {
            printf("ERR (%lx)\n", result);
        }
        else {
            printf("OK\n");
        }
        if (result) {
            if (result >= 0) {
                fprintf(stderr, "Error: Data mismatch at offset %ld\n", result);
            }
            else {
                perror("System error (Verify):");
            }
            break;
        }
    }
    close(fd_sparse);
    close(fd_block);
    free(fiemap);
    return (result ? EXIT_FAILURE : EXIT_SUCCESS);
}

static int write_image(const char *image_file, const char *block_device)
{
    struct stat sbuf;
    if (stat(image_file, &sbuf)) {
        perror("System error (stat image_file):");
        return EXIT_FAILURE;
    }
    if (!S_ISREG(sbuf.st_mode)) {
        fprintf(stderr, "Error: %s is not a regular file\n", image_file);
        return EXIT_FAILURE;
    }
    if (stat(block_device, &sbuf)) {
        perror("System error (stat block_device):");
        return EXIT_FAILURE;
    }
    if (!S_ISBLK(sbuf.st_mode)) {
        fprintf(stderr, "Error: %s is not a block device\n", block_device);
        return EXIT_FAILURE;
    }
    int fd_sparse, fd_block;
    if ((fd_sparse = open(image_file, O_RDONLY)) < 0) {
        fprintf(stderr, "Error: Cannot open sparse file %s\n", image_file);
        return EXIT_FAILURE;
    }
    if ((fd_block = open(block_device, O_WRONLY)) < 0) {
        fprintf(stderr, "Error: Cannot open block device %s\n", block_device);
        close(fd_sparse);
        return EXIT_FAILURE;
    }
    struct fiemap *fiemap;
    if (!(fiemap = read_fiemap(fd_sparse))) {
        fprintf(stderr, "Error: Unable to read fiemap %s\n", image_file);
        close(fd_block);
        close(fd_sparse);
        return EXIT_FAILURE;
    }
    printf("File %s copy %d extents:\n", image_file, fiemap->fm_mapped_extents);
    printf("#\tOffset          Length           Status\n");
    off_t result = -1;
    for (unsigned i = 0; i < fiemap->fm_mapped_extents; i++) {
        result = copy_extent(fd_block, fd_sparse, fiemap->fm_extents[i].fe_logical, fiemap->fm_extents[i].fe_length);
        printf("%d:\t%-16.16llx %-16.16llx %s\n",
               i,
               fiemap->fm_extents[i].fe_logical,
               fiemap->fm_extents[i].fe_length,
               result ? "FAIL" : "OK");
        if (result) {
            if (errno)
                perror("System error (Write copy_extent):");
            break;
        }
    }
    free(fiemap);
    // Sync block filesystem
    syncfs(fd_block);
    // Re-read partition table on the device
    if (ioctl(fd_block, BLKRRPART, NULL)) {
        fprintf(stderr, "Warning: Unable to re-read kernel partition table\n");
    }
    close(fd_block);
    close(fd_sparse);
    return result ? EXIT_FAILURE : EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    const char *img_file, *blk_dev;
    if (argc == 3) {
        img_file = argv[1];
        blk_dev  = argv[2];
    }
    else {
        syntax(argv);
        return EXIT_FAILURE;
    }
    if (write_image(img_file, blk_dev)) {
        return EXIT_FAILURE;
    }
    int result = verify_image(img_file, blk_dev);
    fprintf(stderr, "Write image %s to %s %s\n", img_file, blk_dev, result ? "FAILED" : "SUCCESS");
    return result;
}
