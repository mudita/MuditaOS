#include "lfs.h"
#include "lfs_ioaccess.h"
#include "parse_partitions.h"
#include "parse_args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <dirent.h>

static void create_dir_in_lfs(lfs_t lfs, const char *src)
{
    char *path;
    int ret;

    path = strchr(src, '/');
    if (path) {
        fprintf(stdout, "%s\r\n", path);

        if ((ret = lfs_mkdir(&lfs, path)) < 0) {
            fprintf(stderr, "can't create directory %s: error=%d\r\n", path, ret);
            exit(1);
        }
    }
}

static void create_file_in_lfs(lfs_t lfs, const char *src)
{
    char *path;
    int ret;

    path = strchr(src, '/');
    if (path) {
        fprintf(stdout, "%s\r\n", path);

        // Open source file
        FILE *srcf = fopen(src, "rb");
        if (!srcf) {
            fprintf(stderr, "can't open source file %s: errno=%d (%s)\r\n", src, errno, strerror(errno));
            exit(1);
        }

        // Open destination file
        lfs_file_t dstf;
        if ((ret = lfs_file_open(&lfs, &dstf, path, LFS_O_WRONLY | LFS_O_CREAT)) < 0) {
            fprintf(stderr, "can't open destination file %s: error=%d\r\n", path, ret);
            exit(1);
        }

        char c = fgetc(srcf);
        while (!feof(srcf)) {
            ret = lfs_file_write(&lfs, &dstf, &c, 1);
            if (ret < 0) {
                fprintf(stderr, "can't write to destination file %s: error=%d\r\n", path, ret);
                exit(1);
            }
            c = fgetc(srcf);
        }

        // Close destination file
        ret = lfs_file_close(&lfs, &dstf);
        if (ret < 0) {
            fprintf(stderr, "can't close destination file %s: error=%d\r\n", path, ret);
            exit(1);
        }

        // Close source file
        fclose(srcf);
    }
}
static void add_directory_to_lfs(lfs_t lfs, char *src)
{
    DIR *dir;
    struct dirent *ent;
    char curr_path[PATH_MAX];

    dir = opendir(src);
    if (dir) {
        while ((ent = readdir(dir))) {
            // Skip . and .. directories
            if ((strcmp(ent->d_name, ".") != 0) && (strcmp(ent->d_name, "..") != 0)) {
                // Update the current path
                strcpy(curr_path, src);
                strcat(curr_path, "/");
                strcat(curr_path, ent->d_name);

                if (ent->d_type == DT_DIR) {
                    create_dir_in_lfs(lfs, curr_path);
                    add_directory_to_lfs(lfs, curr_path);
                }
                else if (ent->d_type == DT_REG) {
                    create_file_in_lfs(lfs, curr_path);
                }
            }
        }
        closedir(dir);
    }
}

static void configure_lfs_params(struct lfs_config *lfsc, const struct littlefs_opts *opts)
{
    memset(lfsc, 0, sizeof *lfsc);
    lfsc->block_size     = opts->block_size;
    lfsc->read_size      = opts->read_size;
    lfsc->prog_size      = opts->prog_size;
    lfsc->lookahead_size = opts->lockahead_size;
    lfsc->cache_size     = opts->cache_size;
    lfsc->block_cycles   = opts->block_cycles;
}

int main(int argc, char **argv)
{

    int err;
    struct littlefs_opts lopts;
    struct lfs_config cfg;
    struct lfs_ioaccess_context *ioctx = NULL;
    lfs_t lfs;

    err = parse_program_args(argc, argv, &lopts);
    if (err < 0) {
        return err;
    }
    if (lopts.mode == littlefs_opts_listparts) {
        size_t elems;
        struct partition *parts = find_partitions(lopts.dst_image, scan_all_partitions, &elems);
        print_partitions(parts, elems);
        free(parts);
        free(lopts.src_dirs);
        return EXIT_SUCCESS;
    }
    if (lopts.verbose) {
        print_config_options(&lopts);
    }
    configure_lfs_params(&cfg, &lopts);
    if (lopts.mode == littlefs_opts_parts) {
        size_t elems;
        struct partition *parts = find_partitions(lopts.dst_image, scan_all_partitions, &elems);
        if (!parts) {
            perror("Unable to list partitions:");
            free(lopts.src_dirs);
            return EXIT_FAILURE;
        }
        if (lopts.partition_num - 1 > (int)elems) {
            fprintf(stderr, "Invalid partition selected. Max partition_num is: %lu\n", elems);
            free(parts);
            free(lopts.src_dirs);
            return EXIT_FAILURE;
        }
        ioctx = lfs_ioaccess_open(&cfg, lopts.dst_image, &parts[lopts.partition_num - 1]);
        if (!ioctx) {
            perror("Unable to open file:");
            free(parts);
            free(lopts.src_dirs);
            return EXIT_FAILURE;
        }
        free(parts);
    }
    else if (lopts.mode == littlefs_opts_file) {
        err = truncate(lopts.dst_image, lopts.filesystem_size);
        if (err) {
            perror("Unable to create file:");
            free(lopts.src_dirs);
            return EXIT_FAILURE;
        }
        ioctx = lfs_ioaccess_open(&cfg, lopts.dst_image, NULL);
        if (!ioctx) {
            perror("Unable to open file:");
            free(lopts.src_dirs);
            return EXIT_FAILURE;
        }
    }
    else {
        fprintf(stderr, "Unknown option\n");
        free(lopts.src_dirs);
        return EXIT_FAILURE;
    }

    err = lfs_format(&lfs, &cfg);
    if (err < 0) {
        fprintf(stderr, "lfs format error: error=%d\r\n", err);
        lfs_ioaccess_close(ioctx);
        free(lopts.src_dirs);
        return EXIT_FAILURE;
    }

    err = lfs_mount(&lfs, &cfg);
    if (err < 0) {
        fprintf(stderr, "lfs mount error: error=%d\r\n", err);
        lfs_ioaccess_close(ioctx);
        free(lopts.src_dirs);
        return EXIT_FAILURE;
    }
    for (size_t ndir = 0; ndir < lopts.src_dirs_siz; ++ndir) {
        add_directory_to_lfs(lfs, lopts.src_dirs[ndir]);
    }
    err = lfs_unmount(&lfs);
    if (err < 0) {
        fprintf(stderr, "lfs umount error: error=%d\r\n", err);
        lfs_ioaccess_close(ioctx);
        free(lopts.src_dirs);
        return EXIT_FAILURE;
    }
    free(lopts.src_dirs);
    lfs_ioaccess_close(ioctx);
    return EXIT_SUCCESS;
}
