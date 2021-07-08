// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/SwitchData.hpp>
#include <service-desktop/DesktopMessages.hpp>

#include <filesystem>
#include <utility>

namespace gui
{

    class UpdateSwitchData : public gui::SwitchData
    {
      public:
        explicit UpdateSwitchData(sdesktop::UpdateOsMessage *messageToCopyFrom) : updateOsMessage(*messageToCopyFrom)
        {}

        [[nodiscard]] const sdesktop::UpdateOsMessage &getUpdateOsMessage() const noexcept
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
            osVersion = std::move(version);
        }
    };

} // namespace gui
