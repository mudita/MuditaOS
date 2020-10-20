// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MODULE_SERVICES_SERVICE_EINK_MESSAGES_IMAGEMESSAGE_HPP_
#define MODULE_SERVICES_SERVICE_EINK_MESSAGES_IMAGEMESSAGE_HPP_

#include <cstdint>
#include "EinkMessage.hpp"

namespace seink
{

    /*
     *
     */
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
        virtual ~ImageMessage();

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

#endif /* MODULE_SERVICES_SERVICE_EINK_MESSAGES_IMAGEMESSAGE_HPP_ */
