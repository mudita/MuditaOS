#pragma once

#include "Application.hpp"
#include "Service/Message.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Progress.hpp"

namespace app
{

    inline const std::string name_viewer = "ApplicationViewer";

    class ApplicationViewer : public Application
    {

      public:
        ApplicationViewer(std::string name,
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
