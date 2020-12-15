// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/Common.hpp>
#include "service-gui/source/FrameBufferFlyweight.hpp"

namespace service::eink
{
    struct ImageData
    {
        gui::FrameBufferFlyweight context;
        gui::RefreshModes mode = gui::RefreshModes::GUI_REFRESH_FAST;
        gui::Size size;
        gui::Point point;
    };
} // namespace service::eink

static_assert(std::is_trivially_copyable<service::eink::ImageData>(), "ImageData is POD by default!");
