// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"

#include <bsp/common.hpp>

namespace gui::window::name
{
    inline const std::string bluetooth   = "Bluetooth";
    inline const std::string all_devices = "AllDevices";
    inline const std::string phone_name  = "PhoneName";
    inline const std::string add_device  = "AddDevice";

    inline const std::string network        = "Network";
    inline const std::string phone_modes    = "PhoneModes";
    inline const std::string apps_and_tools = "AppsAndTools";
    inline const std::string security       = "Security";
    inline const std::string system         = "System";

    inline const std::string display_light  = "DisplayLight";
    inline const std::string font_size      = "FontSize";
    inline const std::string keypad_light   = "KeypadLight";
    inline const std::string input_language = "InputLanguage";
    inline const std::string locked_screen  = "LockedScreen";

    inline const std::string messages  = "Messages";
    inline const std::string torch     = "Torch";
    inline const std::string templates = "Templates";

    inline const std::string display_and_keypad = "DisplayAndKeypad";

    inline const std::string change_settings = "ChangeSettings";
    inline const std::string all_operators   = "AllOperators";
    inline const std::string import_contacts = "ImportContacts";
    inline const std::string dialog_settings = "DialogSettings";

} // namespace gui::window::name

namespace app
{
    inline const std::string name_settings_new = "ApplicationSettingsNew";

    class ApplicationSettingsNew : public app::Application
    {
      public:
        ApplicationSettingsNew(std::string name    = name_settings_new,
                               std::string parent  = "",
                               bool startBackgound = false);
        auto DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) -> sys::Message_t override;
        auto InitHandler() -> sys::ReturnCodes override;

        auto SwitchPowerModeHandler(const sys::ServicePowerMode mode) -> sys::ReturnCodes final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;
        bsp::Board board = bsp::Board::none;
    };

} /* namespace app */
