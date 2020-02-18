/*
 * ApplicationAntenna.hpp
 *
 *  Created on: 18 lut 2020
 *      Author: kuba
 */

#ifndef MODULE_APPS_APPLICATION_ANTENNA_APPLICATIONANTENNA_HPP_
#define MODULE_APPS_APPLICATION_ANTENNA_APPLICATIONANTENNA_HPP_

#include "Application.hpp"
#include "windows/AppWindow.hpp"

namespace app
{

    inline const std::string name_antenna = "ApplicationAntenna";

    class ApplicationAntenna : public app::Application
    {
      public:
        ApplicationAntenna(std::string name = name_antenna, std::string parent = "", bool startBackgound = false);
        virtual ~ApplicationAntenna();

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

#endif /* MODULE_APPS_APPLICATION_ANTENNA_APPLICATIONANTENNA_HPP_ */
