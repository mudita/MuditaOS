#pragma once

#include <string>

#include <Application.hpp>

namespace app
{
    inline const std::string name_meditation = "ApplicationMeditation";

    class ApplicationMeditation : public Application
    {
      public:
        explicit ApplicationMeditation(std::string name     = name_meditation,
                                       std::string parent   = {},
                                       bool startBackground = false);

        auto InitHandler() -> sys::ReturnCodes override;
        auto DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) -> sys::Message_t override;
        auto SwitchPowerModeHandler(sys::ServicePowerMode mode) -> sys::ReturnCodes final;

        void createUserInterface() override;
        void destroyUserInterface() override;
    };
} // namespace app
