// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-evtmgr/screen-light-control/ScreenLightControlParameters.hpp>

#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <utility>

namespace sevm
{
    class ScreenLightControlMessage : public sys::DataMessage
    {
        const screen_light_control::Action action;

      public:
        explicit ScreenLightControlMessage(screen_light_control::Action act)
            : sys::DataMessage(MessageType::ScreenLightControlAction), action(act)
        {}

        [[nodiscard]] auto getAction() const noexcept -> screen_light_control::Action
        {
            return action;
        }
    };

    class ScreenLightSetAutoModeParams : public ScreenLightControlMessage
    {
        screen_light_control::AutomaticModeParameters params;

      public:
        explicit ScreenLightSetAutoModeParams(screen_light_control::AutomaticModeParameters params)
            : ScreenLightControlMessage(screen_light_control::Action::setAutomaticModeParameters), params{std::move(
                                                                                                       params)}
        {}

        [[nodiscard]] auto getParams() const noexcept -> const screen_light_control::AutomaticModeParameters &
        {
            return params;
        }
    };

    class ScreenLightSetManualModeParams : public ScreenLightControlMessage
    {
        screen_light_control::ManualModeParameters params;

      public:
        explicit ScreenLightSetManualModeParams(screen_light_control::ManualModeParameters params)
            : ScreenLightControlMessage(screen_light_control::Action::setManualModeBrightness), params{params}
        {}

        [[nodiscard]] auto getParams() const noexcept -> screen_light_control::ManualModeParameters
        {
            return params;
        }
    };

    class ScreenLightControlRequestParameters : public sys::DataMessage
    {
      public:
        ScreenLightControlRequestParameters() : sys::DataMessage(MessageType::ScreenLightControlParameters)
        {}
    };

    class ScreenLightControlParametersResponse : public sys::DataMessage
    {
        bool lightOn;
        screen_light_control::ScreenLightMode mode;
        screen_light_control::ManualModeParameters parameters;

      public:
        ScreenLightControlParametersResponse(bool lightOn,
                                             screen_light_control::ScreenLightMode mode,
                                             screen_light_control::ManualModeParameters params)
            : sys::DataMessage(MessageType::ScreenLightControlParametersResponse), lightOn(lightOn), mode(mode),
              parameters(params)
        {}

        [[nodiscard]] bool isLightOn() const noexcept
        {
            return lightOn;
        }
        [[nodiscard]] auto getMode() const noexcept -> screen_light_control::ScreenLightMode
        {
            return mode;
        }
        [[nodiscard]] auto getParams() const noexcept -> screen_light_control::ManualModeParameters
        {
            return parameters;
        }
    };
} // namespace sevm
