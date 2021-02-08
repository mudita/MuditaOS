// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Common.hpp>
#include <Service/Message.hpp>
#include <Service/Service.hpp>

#include "EinkDisplay.hpp"

#include <service-db/DBServiceName.hpp>
#include <service-gui/Common.hpp>

#include <chrono>
#include <cstdint>
#include <string>

namespace service::eink
{
    class ServiceEink : public sys::Service
    {
      public:
        explicit ServiceEink(const std::string &name = service::name::eink, std::string parent = {});

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

        /// It takes 25ms to get a new measurement
        enum class WaveformTemperature
        {
            KEEP_CURRENT,
            MEASURE_NEW,
        };

        void setState(State state) noexcept;
        bool isInState(State state) const noexcept;

        void enterActiveMode();
        void suspend();
        void updateDisplay(std::uint8_t *frameBuffer, ::gui::RefreshModes refreshMode);
        void prepareDisplay(::gui::RefreshModes refreshMode, WaveformTemperature behaviour);

        sys::MessagePointer handleEinkModeChangedMessage(sys::Message *message);
        sys::MessagePointer handleImageMessage(sys::Message *message);
        sys::MessagePointer handlePrepareEarlyRequest(sys::Message *message);

        EinkDisplay display;
        State currentState;

        /*
         * PowerOffTimer to be implemented when needed.
         * It should power off the display when not used for 3000ms.
         */
    };
} // namespace service::eink

namespace sys
{
    template <> struct ManifestTraits<service::eink::ServiceEink>
    {
        static auto GetManifest() -> ServiceManifest
        {
            ServiceManifest manifest;
            manifest.name         = service::name::eink;
            manifest.dependencies = {service::name::db, service::name::gui};
            return manifest;
        }
    };
} // namespace sys
