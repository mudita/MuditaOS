#pragma once
#include "Application.hpp"
#include "Service/Message.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Progress.hpp"

namespace app
{

    class ApplicationClock : public Application
    {
        AppTimer timerClock;
        void timerClockCallback();

      public:
        ApplicationClock(std::string name,
                         std::string parent            = "",
                         uint32_t stackDepth           = 4096,
                         sys::ServicePriority priority = sys::ServicePriority::Idle);

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

} /* namespace app */
