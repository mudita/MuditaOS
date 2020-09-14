#pragma once

#include "Application.hpp"

#include "bsp/common.hpp"

namespace app
{

    inline const std::string name_settings_new = "ApplicationSettingsNew";

    class ApplicationSettingsNew : public app::Application
    {
      public:
        ApplicationSettingsNew(std::string name    = name_settings_new,
                               std::string parent  = "",
                               bool startBackgound = false);
        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;
        bsp::Board board = bsp::Board::none;
    };

} /* namespace app */
