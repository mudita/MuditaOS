// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <vfs.hpp>
#include <i18/i18.hpp>

class vfs vfs;       // needed for compilation, our vfs is global
utils::i18 localize; // needed to load any keymap - these are stored in i18

struct vfs_initializer
{
    vfs_initializer()
    {
        vfs.Init();
    }
} vfs_initializer;
