// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "EinkMessage.hpp"

#include <cstdint>

namespace service::eink
{

    class ImageMessage : public EinkMessage
    {
      protected:
        uint32_t x, y, w, h;
        bool deepRefresh;
        uint8_t *data;
        bool suspend  = false;
        bool shutdown = false;

      public:
        ImageMessage(uint32_t x,
                     uint32_t y,
                     uint32_t w,
                     uint32_t h,
                     bool deepRefresh,
                     uint8_t *data,
                     bool suspend,
                     bool shutdown);

        uint8_t *getData()
        {
            return data;
        };
        uint32_t getSize()
        {
            return w * h;
        };
        bool getDeepRefresh()
        {
            return deepRefresh;
        };
        bool getSuspend()
        {
            return suspend;
        };
        bool getShutdown()
        {
            return shutdown;
        };
    };

} /* namespace seink */
