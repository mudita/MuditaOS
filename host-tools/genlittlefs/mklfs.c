#include "lfs.h"
#include "parse_partitions.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>

static struct lfs_config cfg;
static lfs_t lfs;


static void create_dir(char *src)
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

static void create_file(char *src)
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

static void compact(char *src)
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
                    create_dir(curr_path);
                    compact(curr_path);
                }
                else if (ent->d_type == DT_REG) {
                    create_file(curr_path);
                }
            }
        }

        closedir(dir);
    }
}

void usage()
{
    fprintf(stdout,
            "usage: mklfs -c <pack-dir> -b <block-size> -r <read-size> -p <prog-size> -s <filesystem-size> -i "
            "<image-file-path>\r\n");
}

static int is_number(const char *s)
{
    const char *c = s;

    while (*c) {
        if ((*c < '0') || (*c > '9')) {
            return 0;
        }
        c++;
    }

    return 1;
}

static int is_hex(const char *s)
{
    const char *c = s;

    if (*c++ != '0') {
        return 0;
    }

    if (*c++ != 'x') {
        return 0;
    }

    while (*c) {
        if (((*c < '0') || (*c > '9')) && ((*c < 'A') || (*c > 'F')) && ((*c < 'a') || (*c > 'f'))) {
            return 0;
        }
        c++;
    }

    return 1;
}

static int to_int(const char *s)
{
    if (is_number(s)) {
        return atoi(s);
    }
    else if (is_hex(s)) {
        return (int)strtol(s, NULL, 16);
    }

    return -1;
}

int main(int argc, char **argv)
{

    char *src = NULL;   // Source directory
    char *dst = NULL;   // Destination image
    int c;              // Current option
    int block_size = 0; // Block size
    int read_size  = 0; // Read size
    int prog_size  = 0; // Prog size
    int fs_size    = 0; // File system size
    int err;

    size_t elems;
    struct partition *parts = find_partitions("PurePhone.img", scan_all_partitions, &elems);
    for (size_t i = 0; i < elems; ++i) {
        printf("Start %li end %li type %i\n", parts[i].start, parts[i].end, parts[i].type);
    }
    free(parts);

    while ((c = getopt(argc, argv, "c:i:b:p:r:s:")) != -1) {
        switch (c) {
        case 'c':
            src = optarg;
            break;

        case 'i':
            dst = optarg;
            break;

        case 'b':
            block_size = to_int(optarg);
            break;

        case 'p':
            prog_size = to_int(optarg);
            break;

        case 'r':
            read_size = to_int(optarg);
            break;

        case 's':
            fs_size = to_int(optarg);
            break;
        }
    }

    if ((src == NULL) || (dst == NULL) || (block_size <= 0) || (prog_size <= 0) || (read_size <= 0) || (fs_size <= 0)) {
        usage();
        exit(1);
    }

    cfg.block_size     = block_size;
    cfg.read_size      = read_size;
    cfg.prog_size      = prog_size;
    cfg.block_count    = fs_size / cfg.block_size;
    cfg.lookahead_size = cfg.block_count;
    cfg.context        = NULL;
    cfg.cache_size     = block_size;
    cfg.block_cycles   = 512;

    /*
    FILE *img = fopen(dst, "wb+");

    if (!img) {
        fprintf(stderr, "can't create image file: errno=%d (%s)\r\n", errno, strerror(errno));
        return -1;
    }

    fwrite(data, 1, fs_size, img);

    fclose(img);

    data = calloc(1, fs_size);
    if (!data) {
        fprintf(stderr, "no memory for mount\r\n");
        return -1;
    }
*/
    err = lfs_format(&lfs, &cfg);
    if (err < 0) {
        fprintf(stderr, "format error: error=%d\r\n", err);
        return -1;
    }

    err = lfs_mount(&lfs, &cfg);
    if (err < 0) {
        fprintf(stderr, "mount error: error=%d\r\n", err);
        return -1;
    }

    compact(src);


    return 0;
}
