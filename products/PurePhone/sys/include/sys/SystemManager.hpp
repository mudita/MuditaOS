// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-sys/PhoneModes/Subject.hpp>
#include <module-sys/SystemManager/SystemManagerCommon.hpp>

namespace sys
{
    class PhoneModeRequest;
    class TetheringStateRequest;
    class TetheringEnabledResponse;

    class SystemManager : public SystemManagerCommon
    {
      public:
        explicit SystemManager(std::vector<std::unique_ptr<BaseServiceCreator>> &&creators);

        void StartSystem(InitFunction sysInit, InitFunction appSpaceInit) override;

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

        std::unique_ptr<phone_modes::Subject> phoneModeSubject;
    };
} // namespace sys
