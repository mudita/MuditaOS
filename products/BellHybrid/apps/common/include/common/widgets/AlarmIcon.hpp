// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/Style.hpp>
#include <gui/widgets/ImageBox.hpp>

namespace gui
{
    class ImageBox;

    class AlarmIcon : public ImageBox
    {
      public:
        enum class Status
        {
            UNKNOWN,
            RINGING,
            ACTIVATED,
            DEACTIVATED,
            SNOOZE
        };

        explicit AlarmIcon(Item *parent = nullptr, Position x = 0U, Position y = 0U, Length w = 0U, Length h = 0U);

        auto setStatus(Status status) noexcept -> void;
    };
} /* namespace gui */
