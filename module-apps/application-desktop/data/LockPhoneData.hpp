// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef MODULE_APPS_APPLICATION_DESKTOP_DATA_LOCKPHONEDATA_HPP_
#define MODULE_APPS_APPLICATION_DESKTOP_DATA_LOCKPHONEDATA_HPP_

#include "gui/SwitchData.hpp"
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <filesystem>

#include "application-desktop/widgets/PinLock.hpp"

namespace gui
{

    // class template that stores information that was sent along with switch message
    class LockPhoneData : public gui::SwitchData
    {
        std::string previousApplication;
        std::unique_ptr<PinLock> lock;

      public:
        LockPhoneData(std::unique_ptr<PinLock> &&lock) : SwitchData(), lock(std::move(lock))
        {
            description = "LockPhoneData";
        }

        virtual ~LockPhoneData(){};

        void setPrevApplication(const std::string &prevApp)
        {
            previousApplication = prevApp;
        };
        [[nodiscard]] const std::string &getPreviousApplication()
        {
            return previousApplication;
        };

        [[nodiscard]] std::unique_ptr<PinLock> getLock()
        {
            return std::make_unique<PinLock>(*lock.get());
        }
    };

    class UpdateSwitchData : public gui::SwitchData
    {
      public:
        UpdateSwitchData(sdesktop::UpdateOsMessage *messageToCopyFrom) : updateOsMessage(*messageToCopyFrom)
        {}

        const sdesktop::UpdateOsMessage &getUpdateOsMessage()
        {
            return updateOsMessage;
        }

      private:
        sdesktop::UpdateOsMessage updateOsMessage;
    };

} // namespace gui

#endif /* MODULE_APPS_APPLICATION_DESKTOP_DATA_LOCKPHONEDATA_HPP_ */
