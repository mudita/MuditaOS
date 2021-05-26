// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MODULE_APPS_APPLICATION_SETTINGS_APPLICATIONSETTINGS_HPP_
#define MODULE_APPS_APPLICATION_SETTINGS_APPLICATIONSETTINGS_HPP_

#include "Application.hpp"
#include "bsp/common.hpp"
#include <common_data/EventStore.hpp>

namespace app
{
    inline constexpr auto name_settings  = "ApplicationSettings";

    class ApplicationSettings : public app::Application
    {
      public:
        ApplicationSettings(std::string name                    = name_settings,
                            std::string parent                  = {},
                            sys::phone_modes::PhoneMode mode    = sys::phone_modes::PhoneMode::Connected,
                            StartInBackground startInBackground = {false});
        virtual ~ApplicationSettings();
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;
        bsp::Board board = bsp::Board::none;
        void timeDateChanged(std::string value);

      private:
        bool europeanDateTimeFormat = false; // true europe format, false american format
    };

    template <> struct ManifestTraits<ApplicationSettings>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch, manager::actions::PhoneModeChanged}};
        }
    };
} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_SETTINGS_APPLICATIONSETTINGS_HPP_ */
