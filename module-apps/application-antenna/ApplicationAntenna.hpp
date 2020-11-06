// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

/*
 * ApplicationAntenna.hpp
 *
 *  Created on: 18 lut 2020
 *      Author: kuba
 */

#ifndef MODULE_APPS_APPLICATION_ANTENNA_APPLICATIONANTENNA_HPP_
#define MODULE_APPS_APPLICATION_ANTENNA_APPLICATIONANTENNA_HPP_

#include "Application.hpp"
#include "service-cellular/api/CellularServiceAPI.hpp"
#include "windows/AppWindow.hpp"

namespace app
{

    inline constexpr auto name_antenna = "ApplicationAntenna";
    namespace antenna
    {
        class StoreParams
        {
          public:
            std::string csq;
            std::string creg;
            std::string bandFreq;
        };
        constexpr uint32_t paramsMaxSize = 9;
    } // namespace antenna

    class ApplicationAntenna : public app::Application
    {
      protected:
        std::unique_ptr<sys::Timer> appTimer;
        void timerHandler(void);
        bool cellularRequestInProgress = false;
        bsp::cellular::antenna antenna;

        bool csqUpdated   = false;
        uint32_t lastCsq  = 0;
        uint32_t lastCreg = 0;
        uint32_t lastFreq = 0;

      public:
        ApplicationAntenna(std::string name                    = name_antenna,
                           std::string parent                  = {},
                           StartInBackground startInBackground = {false});
        virtual ~ApplicationAntenna();

        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;
        void setScanInProgress(bool value)
        {
            cellularRequestInProgress = value;
        };
        void handleNetworkParams(std::vector<std::string> &data);
        bsp::cellular::antenna getAntenna(void)
        {
            CellularServiceAPI::GetAntenna(this, antenna);
            return antenna;
        }

        std::vector<app::antenna::StoreParams> lowBandParams;
        std::vector<app::antenna::StoreParams> highBandParams;
    };

    template <> struct ManifestTraits<ApplicationAntenna>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch}};
        }
    };
} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_ANTENNA_APPLICATIONANTENNA_HPP_ */
