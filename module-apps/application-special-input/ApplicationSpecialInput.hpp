#pragma once

#include "Application.hpp"
#include "SwitchData.hpp"

namespace app
{

    inline const std::string special_input = "ApplicationSpecialInput";
    inline const std::string char_select   = gui::name::window::main_window;

    // app just to provide input selection on UI
    class ApplicationSpecialInput : public app::Application
    {
      public:
        std::string requester = "";

        ApplicationSpecialInput(std::string name = special_input, std::string parent = "", bool startBackgound = true);
        virtual ~ApplicationSpecialInput() = default;

        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;
        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }
        void createUserInterface() override;
        void destroyUserInterface() override;
    };

}; // namespace app
