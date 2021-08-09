// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NetworkTime.hpp"

#include <modem/BaseChannel.hpp>
#include <at/ATFactory.hpp>
#include <at/UrcFactory.hpp>

#include <service-time/service-time/TimeMessage.hpp>

#include <log/Logger.hpp>
namespace cellular::service
{

    void NetworkTime::setChannel(at::BaseChannel *channel)
    {
        this->channel = channel;
    }

    std::shared_ptr<sys::Message> NetworkTime::createSettingsRequest()
    {
        return std::make_shared<stm::message::GetAutomaticDateAndTimeRequest>();
    }

    void NetworkTime::processSettings(bool newValue, bool isOfflineModeOn)
    {
        if (isAutomaticDateAndTime == newValue) {
            return;
        }
        isAutomaticDateAndTime = newValue;

        if (isAutomaticDateAndTime) {
            enableTimeReporting(isOfflineModeOn);
        }
        else {
            disableTimeReporting();
        }
    }

    void NetworkTime::enableTimeReporting(bool isOfflineModeOn)
    {
        if (channel == nullptr) {
            LOG_ERROR("No channel provided. Request ignored");
            return;
        }
        channel->cmd(at::AT::ENABLE_TIME_ZONE_UPDATE);
        channel->cmd(at::AT::SET_TIME_ZONE_REPORTING);
        if (!isOfflineModeOn) {
            channel->cmd(at::AT::CFUN_DISABLE_TRANSMITTING);
            channel->cmd(at::AT::CFUN_FULL_FUNCTIONALITY);
        }
    }

    void NetworkTime::disableTimeReporting()
    {
        if (channel == nullptr) {
            LOG_ERROR("No channel provided. Request ignored");
            return;
        }
        channel->cmd(at::AT::DISABLE_TIME_ZONE_UPDATE);
        channel->cmd(at::AT::DISABLE_TIME_ZONE_REPORTING);
    }
}; // namespace cellular::service
