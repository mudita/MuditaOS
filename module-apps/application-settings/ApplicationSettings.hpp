// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MODULE_APPS_APPLICATION_SETTINGS_APPLICATIONSETTINGS_HPP_
#define MODULE_APPS_APPLICATION_SETTINGS_APPLICATIONSETTINGS_HPP_

#include "Application.hpp"

#include "bsp/common.hpp"

namespace app
{

    inline const std::string name_settings  = "ApplicationSettings";
    inline const std::string sim_select     = "SimSelect";
    inline const std::string change_setting = "ChangeSetting";

    class ApplicationSettings : public app::Application
    {
      public:
        ApplicationSettings(std::string name = name_settings, std::string parent = "", bool startBackgound = false);
        virtual ~ApplicationSettings();
        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;
        bsp::Board board = bsp::Board::none;
    };

} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_SETTINGS_APPLICATIONSETTINGS_HPP_ */
