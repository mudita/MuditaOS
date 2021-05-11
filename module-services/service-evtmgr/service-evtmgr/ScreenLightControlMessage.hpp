// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "module-services/service-evtmgr/screen-light-control/ScreenLightControl.hpp"

#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <utility>

namespace sevm
{
    class ScreenLightControlMessage : public sys::DataMessage
    {
      public:
        explicit ScreenLightControlMessage(screen_light_control::Action act)
            : sys::DataMessage(MessageType::ScreenLightControlAction), action(act)
        {}

        const screen_light_control::Action action;
    };

    class ScreenLightSetParameters : public sys::DataMessage
    {
      public:
        ScreenLightSetParameters(screen_light_control::ParameterizedAction act, screen_light_control::Parameters params)
            : sys::DataMessage(MessageType::ScreenLightControlAction), action(act), parameters(std::move(params))
        {}

        const screen_light_control::ParameterizedAction action;
        const screen_light_control::Parameters parameters;
    };

    class ScreenLightControlRequestParameters : public sys::DataMessage
    {
      public:
        ScreenLightControlRequestParameters() : sys::DataMessage(MessageType::ScreenLightControlParameters)
        {}
    };

    class ScreenLightControlParametersResponse : public sys::DataMessage
    {
      public:
        ScreenLightControlParametersResponse() : sys::DataMessage(MessageType::ScreenLightControlParametersResponse)
        {}

        ScreenLightControlParametersResponse(bool lightOn,
                                             screen_light_control::ScreenLightMode mode,
                                             screen_light_control::Parameters params)
            : sys::DataMessage(MessageType::ScreenLightControlParametersResponse), lightOn(lightOn), mode(mode),
              parameters(std::move(params))
        {}

        bool lightOn;
        screen_light_control::ScreenLightMode mode;
        screen_light_control::Parameters parameters;
    };
} // namespace sevm
