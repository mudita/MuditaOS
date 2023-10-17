// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
        std::optional<screen_light_control::Parameters> params;
        const screen_light_control::Sender sender;

      public:
        explicit ScreenLightControlMessage(screen_light_control::Action act,
                                           std::optional<screen_light_control::Parameters> params = std::nullopt,
                                           screen_light_control::Sender sender = screen_light_control::Sender::Other)
            : sys::DataMessage(MessageType::ScreenLightControlAction),
              action(act), params{std::move(params)}, sender{sender}
        {}

        [[nodiscard]] auto getAction() const noexcept -> screen_light_control::Action
        {
            return action;
        }

        [[nodiscard]] auto getParams() const noexcept -> const std::optional<screen_light_control::Parameters> &
        {
            return params;
        }

        [[nodiscard]] auto getSender() const noexcept -> screen_light_control::Sender
        {
            return sender;
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

    class ScreenLightSetAutoProgressiveModeParams : public ScreenLightControlMessage
    {
        screen_light_control::LinearProgressModeParameters params;

      public:
        explicit ScreenLightSetAutoProgressiveModeParams(screen_light_control::LinearProgressModeParameters params)
            : ScreenLightControlMessage(screen_light_control::Action::setAutomaticModeParameters), params{std::move(
                                                                                                       params)}
        {}

        [[nodiscard]] auto getParams() const noexcept -> const screen_light_control::LinearProgressModeParameters &
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

    class ScreenLightSetConstLinearModeParams : public ScreenLightControlMessage
    {
        screen_light_control::ConstLinearProgressModeParameters params;

      public:
        explicit ScreenLightSetConstLinearModeParams(screen_light_control::ConstLinearProgressModeParameters params)
            : ScreenLightControlMessage(screen_light_control::Action::setAutomaticModeParameters), params{params}
        {}

        [[nodiscard]] auto getParams() const noexcept -> screen_light_control::ConstLinearProgressModeParameters
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

    class ScreenLightControlParametersResponse : public sys::ResponseMessage
    {
        bool lightOn;
        screen_light_control::ScreenLightMode mode;
        screen_light_control::ManualModeParameters parameters;

      public:
        ScreenLightControlParametersResponse(bool lightOn,
                                             screen_light_control::ScreenLightMode mode,
                                             screen_light_control::ManualModeParameters params)
            : lightOn(lightOn), mode(mode), parameters(params)
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
