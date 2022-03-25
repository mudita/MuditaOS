// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <common/models/AbstractAudioModel.hpp>

namespace app::meditation::models
{
    class ChimeInterval;
    class ChimeVolume;
    class StartDelay;
} // namespace app::meditation::models

namespace app
{
    class MeditationTimer : public Application
    {
      public:
        static constexpr auto defaultName = "ApplicationMeditationTimer";

        explicit MeditationTimer(std::string name                    = defaultName,
                                 std::string parent                  = "",
                                 StatusIndicators statusIndicators   = StatusIndicators{},
                                 StartInBackground startInBackground = {false},
                                 uint32_t stackDepth                 = 4096 * 2);
        ~MeditationTimer();

        sys::ReturnCodes InitHandler() override;

        void createUserInterface() override;
        void destroyUserInterface() override
        {}

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

      private:
        std::unique_ptr<app::meditation::models::ChimeInterval> chimeIntervalModel;
        std::unique_ptr<app::meditation::models::ChimeVolume> chimeVolumeModel;
        std::unique_ptr<app::meditation::models::StartDelay> startDelayModel;
        std::unique_ptr<AbstractAudioModel> audioModel;
    };

    template <> struct ManifestTraits<MeditationTimer>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} // namespace app
