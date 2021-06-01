// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

/// empty stub just to compile without including whole module-bt
extern "C"
{
    void bt_log_custom(const char *file, int line, const char *foo, const char *fmt, ...)
    {}

    int log_Log(int level, const char *file, int line, const char *function, const char *fmt, ...)
    {
        return 0;
    }
}
