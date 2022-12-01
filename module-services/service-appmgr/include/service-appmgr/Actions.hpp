// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
        using ActionParams    = gui::SwitchData;
        using ActionParamsPtr = std::unique_ptr<ActionParams>;

        enum Action : ActionId
        {
            Home,
            AutoLock,
            Launch,
            Call,
            ActivateCall,
            HandleOutgoingCall,
            HandleIncomingCall,
            HandleCallerId,
            NotAnEmergencyNotification,
            NoSimNotification,
            NoNetworkConnectionNotification,
            CallRequestGeneralErrorNotification,
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
            ShowAlarm,
            ShowMMIResult,
            ShowMMIResponse,
            ShowMMIPush,
            SmsRejectNoSim,
            DisplayLowBatteryScreen,
            SystemBrownout,
            DisplayLogoAtExit,
            DisplayFactoryResetInProgressScreen,
            SMSRejectedByOfflineNotification,
            CallRejectedByOfflineNotification,
            PhoneModeChanged,
            TetheringStateChanged,
            BluetoothModeChanged,
            AlarmClockStatusChanged,
            NotificationsChanged,
            ShowPopup,
            AbortPopup,
            ChangeHomescreenLayout,
            UserAction // The last enumerator in the Action enum.
                       // All user-defined actions shall have values greater than UserAction.
                       // All system-wide actions shall have values lesser than UserAction.
        };

        enum class ActionFlag
        {
            None,
            AcceptWhenInBackground /// Accepts actions even if in background.
        };

        struct RegisteredAction
        {
            constexpr RegisteredAction(ActionId id, ActionFlag flag = ActionFlag::None) : id{id}, flag{flag}
            {}

            const ActionId id;
            const ActionFlag flag;
        };
        using ActionFilter = std::vector<RegisteredAction>;

        class ConvertibleToAction
        {
          public:
            virtual ~ConvertibleToAction() noexcept = default;

            virtual auto toAction() const -> std::unique_ptr<ActionRequest> = 0;
        };
    } // namespace actions
} // namespace app::manager
