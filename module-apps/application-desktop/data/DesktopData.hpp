// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "gui/SwitchData.hpp"
#include <service-desktop/DesktopMessages.hpp>
#include <service-desktop/ServiceDesktop.hpp>
#include <filesystem>

namespace gui
{

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

    class CurrentOsVersion : public gui::SwitchData
    {
        std::string osVersion;

      public:
        [[nodiscard]] std::string getCurrentOsVersion() const
        {
            return osVersion;
        }

        void setData(std::string version)
        {
            osVersion = version;
        }
    };

} // namespace gui
