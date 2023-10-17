// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <stdbool.h>
#include <stddef.h>

enum littlefs_opts_mode
{
    littlefs_opts_listparts,
    littlefs_opts_parts,
    littlefs_opts_file
};
struct littlefs_opts
{
    //! LFS config options
    int read_size;      //! Read size
    int block_size;     //! block size
    int prog_size;      //! Eraseable area size
    int cache_size;     //! Cache size
    int lockahead_size; //! Lock ahead size
    int block_cycles;   //! Progam counter cycles
    //! Other ars
    enum littlefs_opts_mode mode; //! Software mode
    long long filesystem_size;    //! Filesystem size
    int partition_num;            //! Parition number
    size_t src_dirs_siz;          //! SOurce dirs size
    char **src_dirs;              //! Source directories
    char *dst_image;              //! Destination image
    bool overwrite_existing;      //! Overwrite existing format
    bool verbose;                 //! Verbose mode
};

int parse_program_args(int argc, char **argv, struct littlefs_opts *opts) __attribute__((nonnull(3)));

void print_config_options(const struct littlefs_opts *opts) __attribute__((nonnull(1)));
