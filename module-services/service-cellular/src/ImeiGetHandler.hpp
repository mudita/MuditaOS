// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-cellular/service-cellular/CellularMessage.hpp>

#include <memory>
//#include <vector>

namespace at
{
    class Cmd;
    class BaseChannel;
} // namespace at

namespace sys
{
    class Message;
} // namespace sys

namespace cellular::service
{
    class ImeiGetHandler
    {
      public:
        /** Set cmd channel
         * \param channel channel (or nullptr to block communication)
         */
        void setChannel(at::BaseChannel *channel);
        /**
         * Reads IMEI as a string
         * @param destination reference to data destination
         * @return boor result true on success, false on fail
         */
        auto getImei(std::string &destination) -> bool;

      private:
        /**
         * Reads number of contacts stored on sim card
         * @param count number of contacts stored on sim card
         * @return boor result true on success, false on fail
         */
        at::BaseChannel *channel = nullptr;
    };
} // namespace cellular::service
