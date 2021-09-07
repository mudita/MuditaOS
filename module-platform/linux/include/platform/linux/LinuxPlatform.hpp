// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Platform.hpp>

#include <purefs/vfs_subsystem.hpp>

#include <string>

namespace platform::linux
{
    class LinuxPlatform : public platform::Platform
    {
      public:
        explicit LinuxPlatform(std::string imageName);
        ~LinuxPlatform();

        void init() final;

      private:
        void initFilesystem();

        bool usesFilesystem = false;
        std::string imageName;
        purefs::subsystem::vfs_handle_t vfs;
    };

} // namespace platform::linux
