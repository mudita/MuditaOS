// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <PhoneModes/Subject.hpp>
#include <SystemManager/SystemManagerCommon.hpp>

namespace sys
{
    class PhoneModeRequest;
    class TetheringStateRequest;
    class TetheringEnabledResponse;

    class SystemManager : public SystemManagerCommon
    {
      public:
        explicit SystemManager(std::vector<std::unique_ptr<BaseServiceCreator>> &&creators);

        void StartSystem(InitFunction sysInit, InitFunction appSpaceInit, DeinitFunction sysDeinit) override;

        /// Translates a slider state into a phone mode.
        /// \param key  Slider button state
        /// \return Phone mode.
        static phone_modes::PhoneMode translateSliderState(const RawKey &key);

      private:
        ReturnCodes InitHandler() override;

        MessagePointer handlePhoneModeRequest(PhoneModeRequest *request);
        MessagePointer handleTetheringStateRequest(TetheringStateRequest *request);
        MessagePointer enableTethering(TetheringEnabledResponse *response);
        void batteryNormalLevelAction() override;
        void batteryCriticalLevelAction(bool charging) override;
        void handleShutdown() override;
        const std::vector<std::string> &getWhiteListFor(WhiteListType type) override;

        std::unique_ptr<phone_modes::Subject> phoneModeSubject;
    };
} // namespace sys
