// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"

#include <Option.hpp>

namespace gui
{
    namespace window::name
    {
        inline constexpr auto change_time_zone = "ChangeTimeZone";
    }
    class ChangeTimeZone : public BaseSettingsWindow
    {
      public:
        explicit ChangeTimeZone(app::Application *app);

      protected:
        [[nodiscard]] auto buildOptionsList() -> std::list<Option> override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        [[nodiscard]] auto setTimeZoneIndex() -> unsigned int;
        [[nodiscard]] auto extractTimeZoneName(const std::string &name) const noexcept -> std::string;

        const std::vector<std::string> timeZonesList;
        std::string selectedTimeZone;
    };
} // namespace gui
