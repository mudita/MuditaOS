// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EinkMessage.hpp"

#include <cstdint>
#include <service-eink/ImageData.hpp>

namespace service::eink
{

    class ImageMessage : public EinkMessage
    {
      protected:
        ImageData imageData;
        bool suspend  = false;
        bool shutdown = false;

      public:
        ImageMessage(ImageData &&imageData, bool suspend, bool shutdown)
            : imageData(std::move(imageData)), suspend(suspend), shutdown(shutdown)
        {}

        [[nodiscard]] gui::Context &getContext()
        {
            return imageData.context;
        }

        bool getDeepRefresh()
        {
            return imageData.mode == gui::RefreshModes::GUI_REFRESH_DEEP;
        }

        bool getSuspend()
        {
            return suspend;
        }

        bool getShutdown()
        {
            return shutdown;
        }
    };

} // namespace service::eink
