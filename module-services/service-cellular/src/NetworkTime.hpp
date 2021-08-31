// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>

namespace at
{
    class Cmd;
    class BaseChannel;
} // namespace at

namespace sys
{
    class Message;
}
namespace cellular::service
{
    class NetworkTime
    {
      public:
        /** Set cmd channel
         * \param channel channel (or nullptr to block communication)
         */
        void setChannel(at::BaseChannel *channel);
        /**
         * Requests settings related to Network Time synchronisation
         */
        std::shared_ptr<sys::Message> createSettingsRequest();
        /**
         * Handles settings related to Network Time synchronisation change
         * @param newValue Automatic Date and Time setting value
         * @param isOfflineModeOn is Offline mode currently enabled
         */
        void processSettings(bool newValue, bool isOfflineModeOn);

      private:
        at::BaseChannel *channel    = nullptr;
        bool isAutomaticDateAndTime = false;
        /**
         * It enables Network Time reporting
         * @param isOfflineModeOn is Offline mode currently enabled
         */
        void enableTimeReporting(bool isOfflineModeOn);
        /**
         * It disables Network Time reporting
         */
        void disableTimeReporting();
    };
} // namespace cellular::service
