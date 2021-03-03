// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MODULE_APPS_APPLICATION_SETTINGS_APPLICATIONSETTINGS_HPP_
#define MODULE_APPS_APPLICATION_SETTINGS_APPLICATIONSETTINGS_HPP_

#include "Application.hpp"
#include "bsp/common.hpp"
#include <common_data/EventStore.hpp>

namespace app
{

    inline constexpr auto name_settings  = "ApplicationSettings";
    inline constexpr auto sim_select     = "SimSelect";
    inline constexpr auto change_setting = "ChangeSetting";

    class SimSetter
    {
      public:
        virtual ~SimSetter()                     = default;
        virtual void setSim(Store::GSM::SIM sim) = 0;
    };

    class PinLockSetter
    {
      public:
        virtual ~PinLockSetter()              = default;
        virtual void setPin(unsigned int pin) = 0;
        virtual void clearPin()               = 0;
    };

    class ApplicationSettings : public app::Application, public SimSetter, public PinLockSetter
    {
      public:
        ApplicationSettings(std::string name                    = name_settings,
                            std::string parent                  = {},
                            StartInBackground startInBackground = {false});
        virtual ~ApplicationSettings();
        sys::MessagePointer DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
        sys::ReturnCodes InitHandler() override;
        sys::ReturnCodes DeinitHandler() override;

        sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
        {
            return sys::ReturnCodes::Success;
        }

        void createUserInterface() override;
        void destroyUserInterface() override;
        bsp::Board board = bsp::Board::none;
        void setSim(Store::GSM::SIM sim) override;
        void setPin(unsigned int pin) override;
        void clearPin() override;
        void lockPassChanged(std::string value);
        void timeDateChanged(std::string value);

      private:
        unsigned int lockPassHash;
        bool europeanDateTimeFormat = false; // true europe format, false american format
    };

    template <> struct ManifestTraits<ApplicationSettings>
    {
        static auto GetManifest() -> manager::ApplicationManifest
        {
            return {{manager::actions::Launch, manager::actions::SelectSimCard}};
        }
    };
} /* namespace app */

#endif /* MODULE_APPS_APPLICATION_SETTINGS_APPLICATIONSETTINGS_HPP_ */
