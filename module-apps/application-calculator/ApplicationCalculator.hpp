#pragma once

#include <Application.hpp>

namespace app
{
    const inline std::string name_calculator = "ApplicationCalculator";
    constexpr std::uint16_t stack_size       = 4096;

    class ApplicationCalculator : public Application
    {

      public:
        ApplicationCalculator(std::string name = name_calculator, std::string parent = "", bool startBackgound = false);
        ~ApplicationCalculator() override = default;

        sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() final;
        void destroyUserInterface() final;
    };

} /* namespace app */
