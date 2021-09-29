// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Platform.hpp>

#include <purefs/vfs_subsystem.hpp>

#include <string>

namespace platform::rt1051
{
    class RT1051Platform : public platform::Platform
    {
      public:
        RT1051Platform();

        void init() final;
        void deinit() final;

      private:
        void initFilesystem();

        bool usesFilesystem = false;
        purefs::subsystem::vfs_handle_t vfs;
    };

} // namespace platform::rt1051
