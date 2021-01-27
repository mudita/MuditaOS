// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/SwitchData.hpp>

#include <memory>
#include <string>
#include <vector>

namespace app::manager
{
    class ActionRequest; // Forward declaration.

    namespace actions
    {
        using ActionId        = int;
        using ActionFilter    = std::vector<ActionId>;
        using ActionParams    = gui::SwitchData;
        using ActionParamsPtr = std::unique_ptr<ActionParams>;

        enum Action : ActionId
        {
            Home,
            Launch,
            CloseSystem,
            Call,
            Dial,
            EmergencyDial,
            ShowCallLog,
            CreateSms,
            ShowSmsTemplates,
            ShowContacts,
            ShowEmergencyContacts,
            AddContact,
            EditContact,
            ShowContactDetails,
            ShowSpecialInput,
            SelectSimCard,
            ShowAlarm,
            ShowReminder,
            RequestPin,
            RequestPuk,
            RequestPinChange,
            UnlockSim,
            BlockSim,
            ShowMMIResult,
            ShowMMIResponse,
            ShowMMIPush,
            DisplayCMEError,
            DisplayLowBatteryNotification,
            SystemBrownout,
            UserAction // The last enumerator in the Action enum.
                       // All user-defined actions shall have values greater than UserAction.
                       // All system-wide actions shall have values lesser than UserAction.
        };

        class ConvertibleToAction
        {
          public:
            virtual ~ConvertibleToAction() noexcept = default;

            virtual auto toAction() const -> std::unique_ptr<ActionRequest> = 0;
        };
    } // namespace actions
} // namespace app::manager
