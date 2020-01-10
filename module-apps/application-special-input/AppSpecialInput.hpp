#pragma once

#include "Application.hpp"
#include "SwitchData.hpp"

namespace app
{

    inline const std::string special_input = "AppSpecialInput";
    inline const std::string char_select = gui::name::window::main_window;

    // app just to provide input selection on UI
    class AppSpecialInput : public app::Application
    {
      public:
        std::string requester = "";

        AppSpecialInput(std::string name = special_input, std::string parent = "", bool startBackgound = true);
        virtual ~AppSpecialInput() = default;

        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override
        {
            return sys::ReturnCodes::Success;
        }
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }
        void createUserInterface() override
        {
            return;
        }
        void destroyUserInterface() override
        {
            return;
        }
    };

}; // namespace app
