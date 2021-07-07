// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "parse_args.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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
    if (!s) {
        return -1;
    }
    if (is_number(s)) {
        return atoi(s);
    }
    else if (is_hex(s)) {
        return (int)strtol(s, NULL, 16);
    }

    return -1;
}

static long long to_longlong(const char *s)
{
    if (!s) {
        return -1;
    }
    if (is_number(s)) {
        return atoll(s);
    }
    else if (is_hex(s)) {
        return (long long)strtoll(s, NULL, 16);
    }
    return -1;
}

static void usage(const char *name) __attribute__((nonnull(1)));
static void usage(const char *name)
{
    fprintf(stderr, "usage: %s [options] -- src_dir1 ... [src_dirN]\n", name);
}

static void help(const char *name) __attribute__((nonnull(1)));
static void help(const char *name)
{
    static const char help_text[] = "usage: %s [options] -- src_dir1 ... [src_dirN]\n"
                                    "\n"
                                    "general options:\n"
                                    "    -h   --help              print help\n"
                                    "\n"
                                    "genlilttlefs options:\n"
                                    "    -i   --image             partition file image\n"
                                    "    -b   --block_size        logical block size, overrides the block device\n"
                                    "    -s   --filesystem_size   filesystem size when creating the new file\n"
                                    "    -p   --partition_num     partition number when use partitions (1-n)\n"
                                    "    -l   --list_partitions   list existing parition and numbers\n"
                                    "    --cache_size             size of caches (block_size)\n"
                                    "    --read_size              readable unit (block_size)\n"
                                    "    --prog_size              programmable unit (block_size)\n"
                                    "    --lookahead_size         size of lookahead buffer (8192)\n"
                                    "    --block_cycles           number of erase cycles before eviction (512)\n"
                                    "    --overwrite              reformat existing partition\n"
                                    "    --verbose                verbose mode\n"
                                    "\n"
                                    "positional arguments:\n"
                                    "    src_dir1                 first source directory\n"
                                    "    src_dirN                 N-th source directory\n";
    fprintf(stderr, help_text, name);
}

int parse_program_args(int argc, char **argv, struct littlefs_opts *opts)
{
    int c; // Current option
    int option_index                    = 0;
    bool is_help                        = false;
    bool is_listpart                    = false;
    bool is_unknown                     = false;
    static struct option long_options[] = {
        {.name = "image", .has_arg = required_argument, .flag = 0, .val = 'i'},
        {.name = "block_size", .has_arg = required_argument, .flag = 0, .val = 'b'},
        {.name = "filesystem_size", .has_arg = required_argument, .flag = 0, .val = 's'},
        {.name = "partition_num", .has_arg = required_argument, .flag = 0, .val = 'p'},
        {.name = "list_partitions", .has_arg = no_argument, .flag = 0, .val = 'l'},
        {.name = "help", .has_arg = no_argument, .flag = 0, .val = 'h'},
        {.name = "read_size", .has_arg = required_argument, .flag = 0, .val = 0},
        {.name = "prog_size", .has_arg = required_argument, .flag = 0, .val = 0},
        {.name = "cache_size", .has_arg = required_argument, .flag = 0, .val = 0},
        {.name = "lockahead_size", .has_arg = required_argument, .flag = 0, .val = 0},
        {.name = "block_cycles", .has_arg = required_argument, .flag = 0, .val = 0},
        {.name = "overwrite", .has_arg = no_argument, .flag = 0, .val = 0},
        {.name = "verbose", .has_arg = no_argument, .flag = 0, .val = 0},
        {.name = 0, .has_arg = 0, .flag = 0, .val = 0}};
    memset(opts, 0, sizeof(*opts));
    while ((c = getopt_long(argc, argv, "i:b:s:p:lh", long_options, &option_index)) != -1) {
        switch (c) {
        case 0:
            if (long_options[option_index].flag != 0) {
                break;
            }
            const char *optname = long_options[option_index].name;
            if (!strcmp(optname, "overwrite")) {
                opts->overwrite_existing = true;
            }
            else if (!strcmp(optname, "verbose")) {
                opts->verbose = true;
            }
            else if (!strcmp(optname, "block_cycles")) {
                opts->block_cycles = to_int(optarg);
            }
            else if (!strcmp(optname, "lockahead_size")) {
                opts->lockahead_size = to_int(optarg);
            }
            else if (!strcmp(optname, "cache_size")) {
                opts->cache_size = to_int(optarg);
            }
            else if (!strcmp(optname, "prog_size")) {
                opts->prog_size = to_int(optarg);
            }
            else if (!strcmp(optname, "read_size")) {
                opts->read_size = to_int(optarg);
            }
            break;
        case 'i':
            opts->dst_image = optarg;
            break;
        case 'b':
            opts->block_size = to_int(optarg);
            break;
        case 's':
            opts->filesystem_size = to_longlong(optarg);
            break;
        case 'p':
            opts->partition_num = to_int(optarg);
            break;
        case 'l':
            is_listpart = true;
            break;
        case 'h':
            is_help = true;
            break;
        default:
            is_unknown = true;
            break;
        }
    }
    if (argc == 1) {
        usage(argv[0]);
        return -1;
    }
    else if (is_help) {
        help(argv[0]);
        return -1;
    }
    else if (is_unknown) {
        fprintf(stderr, "Unknown option\n");
        return -1;
    }
    else if (is_listpart) {
        opts->mode = littlefs_opts_listparts;
        if (!opts->dst_image) {
            fprintf(stderr, "--image <file> is not specified\n");
            return -1;
        }
        else {
            return 0;
        }
    }
    else {
        if (opts->block_cycles < 0) {
            fprintf(stderr, "Warning: disable wear leveling\n");
            opts->block_cycles = -1;
        }
        else if (opts->block_cycles > 1000) {
            fprintf(stderr, "Error: Maximum block cycles is 1000\n");
            return -1;
        }
        else if (opts->block_cycles == 0) {
            opts->block_cycles = 512;
        }
        if (opts->block_size == 0) {
            fprintf(stderr, "Missing --block_size <n> argument\n");
            return -1;
        }
        else if (opts->block_size < 0) {
            fprintf(stderr, "argument --block_size <n> need to be > 0\n");
            return -1;
        }
        if (opts->read_size == 0) {
            opts->read_size = opts->block_size;
        }
        else if (opts->read_size < 0) {
            fprintf(stderr, "argument --read_size <n> needs to be >0\n");
            return -1;
        }
        if (opts->prog_size == 0) {
            opts->prog_size = opts->block_size;
        }
        else if (opts->prog_size < 0) {
            fprintf(stderr, "argument --prog_size <n> needs to be >0\n");
            return -1;
        }
        if (opts->cache_size == 0) {
            opts->cache_size = opts->block_size;
        }
        else if (opts->cache_size < 0) {
            fprintf(stderr, "argument --cache_size <n> needs to be >0\n");
            return -1;
        }
        if (opts->lockahead_size == 0) {
            opts->lockahead_size = 8192;
        }
        else if (opts->cache_size < 0) {
            fprintf(stderr, "argument --lockahead_size <n> needs to be >0\n");
            return -1;
        }
        if (!opts->dst_image) {
            fprintf(stderr, "--image <filename> is not specified\n");
            return -1;
        }
        if (opts->filesystem_size < 0) {
            fprintf(stderr, "argument --filesystem_size <n> needs to be >0\n");
            return -1;
        }
        else if (opts->partition_num < 0) {
            fprintf(stderr, "argument --partition_num <n> needs to be >0\n");
            return -1;
        }
        else if (opts->filesystem_size == 0 && opts->partition_num == 0) {
            fprintf(stderr, "Missing --filesystem_size <size> or --partition_num <num>\n");
            return -1;
        }
        else if (opts->filesystem_size > 0 && opts->partition_num > 0) {
            fprintf(stderr, "Only --filesystem_size or --partition_num is allowed in same time");
            return -1;
        }
        else if (opts->filesystem_size > 0) {
            if (opts->filesystem_size % opts->block_size) {
                fprintf(stderr, "--filesystem_size <size> should be multiply of block size\n");
                return -1;
            }
            opts->mode = littlefs_opts_file;
        }
        else if (opts->partition_num > 0) {
            opts->mode = littlefs_opts_parts;
        }
        if (optind < argc) {
            opts->src_dirs_siz = argc - optind;
            opts->src_dirs     = calloc(opts->src_dirs_siz, sizeof(char *));
            for (int i = 0; optind < argc; ++i) {
                char *arg           = argv[optind++];
                const size_t arg_sz = strlen(arg);
                if (arg[arg_sz - 1] == '/')
                    arg[arg_sz - 1] = '\0';
                opts->src_dirs[i] = arg;
            }
        }
        else {
            opts->src_dirs_siz = 0;
            opts->src_dirs     = NULL;
        }
        if (!opts->dst_image) {
            fprintf(stderr, "--image <file> is not specified\n");
            return -1;
        }
    }
    return 0;
}

void print_config_options(const struct littlefs_opts *opts)
{
    static const char struct_info[] = "genlittlefs configuration:\n"
                                      "   LFS read size %i\n"
                                      "   LFS block size: %i\n"
                                      "   LFS prog size: %i\n"
                                      "   LFS cache size: %i\n"
                                      "   LFS lookahead size: %i\n"
                                      "   LFS block cycles: %i\n"
                                      "   Filesystem size: %lli\n"
                                      "   Partition number: %i\n"
                                      "   Overwrite existing fs: %i\n";
    printf(struct_info,
           opts->read_size,
           opts->block_size,
           opts->prog_size,
           opts->cache_size,
           opts->lockahead_size,
           opts->block_cycles,
           opts->filesystem_size,
           opts->partition_num,
           opts->overwrite_existing);
}
