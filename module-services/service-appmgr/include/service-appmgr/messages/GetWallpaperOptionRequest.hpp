// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/data/WallpaperOption.hpp>

namespace app::manager
{
    class GetWallpaperOptionRequest : public sys::DataMessage
    {};

    class GetWallpaperOptionResponse : public sys::ResponseMessage
    {
      public:
        explicit GetWallpaperOptionResponse(gui::WallpaperOption option) : option{option}
        {}

        const auto getWallpaperOption() -> gui::WallpaperOption
        {
            return option;
        }

      private:
        gui::WallpaperOption option;
    };
} // namespace app::manager
