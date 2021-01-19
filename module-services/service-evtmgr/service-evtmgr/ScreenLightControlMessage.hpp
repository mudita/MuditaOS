// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
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
        ScreenLightControlMessage() : sys::DataMessage(MessageType::ScreenLightControlAction)
        {}
        ScreenLightControlMessage(screen_light_control::Action act,
                                  screen_light_control::Parameters params = screen_light_control::Parameters())
            : sys::DataMessage(MessageType::ScreenLightControlAction), action(act), parameters(std::move(params))
        {}

        screen_light_control::Action action;
        screen_light_control::Parameters parameters;
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
