// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <iosyscalls.hpp>

auto constexpr TESTS_SYSROOT = "test-sysroot";
auto constexpr IMAGE_NAME    = "Test.img";

void __attribute__((constructor)) setup_sysroot()
{
    iosyscalls_set_sysroot(TESTS_SYSROOT);
}

void __attribute__((constructor)) setup_disk_image()
{
    iosyscalls_set_image_name(IMAGE_NAME);
}
