// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/core/Context.hpp>
#include <system/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>
#include <Timers/TimerHandle.hpp>

#include "EinkSentinel.hpp"

#include <service-db/DBServiceName.hpp>
#include <service-db/Settings.hpp>

#include <chrono>
#include <cstdint>
#include <string>
#include <module-services/service-eink/messages/EinkModeMessage.hpp>
#include <hal/eink/AbstractEinkDisplay.hpp>

#include "Common.hpp"

namespace service::eink
{
    enum ExitAction
    {
        None,
        WipeOut
    };

    class ServiceEink : public sys::Service
    {
      public:
        explicit ServiceEink(ExitAction exitAction   = WipeOut,
                             const std::string &name = service::name::eink,
                             std::string parent      = {});

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *response) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override;

      private:
        enum class State
        {
            Running,
            Suspended
        };

        enum class EinkDisplayState
        {
            Idle,
            NeedRefresh,
            Canceled
        };

        void setState(State state) noexcept;
        bool isInState(State state) const noexcept;

        void enterActiveMode();
        void suspend();

        void setDisplayMode(EinkModeMessage::Mode mode);

        sys::MessagePointer handleEinkModeChangedMessage(sys::Message *message);
        sys::MessagePointer handleImageMessage(sys::Message *message);
        sys::MessagePointer handlePrepareEarlyRequest(sys::Message *message);

        void initStaticData();

        ExitAction exitAction;
        State currentState;
        std::unique_ptr<hal::eink::AbstractEinkDisplay> display;
        sys::TimerHandle displayPowerOffTimer;
        std::shared_ptr<EinkSentinel> eInkSentinel;
        std::unique_ptr<settings::Settings> settings;

        std::unique_ptr<::gui::Context> previousContext;
        hal::eink::EinkRefreshMode previousRefreshMode = hal::eink::EinkRefreshMode::REFRESH_NONE;
        EinkDisplayState einkDisplayState              = EinkDisplayState::Idle;
        hal::eink::EinkFrame refreshFramesSum;
        hal::eink::EinkRefreshMode refreshModeSum = hal::eink::EinkRefreshMode::REFRESH_NONE;
        bool isRefreshFramesSumValid              = false;
    };
} // namespace service::eink

namespace sys
{
    template <>
    struct ManifestTraits<service::eink::ServiceEink>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::eink;
            manifest.dependencies = {service::name::db};
            return manifest;
        }
    };
} // namespace sys
