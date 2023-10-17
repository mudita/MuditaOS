// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace gui
{

    class ChangeTimeZone : public BaseSettingsWindow
    {
      public:
        explicit ChangeTimeZone(app::ApplicationCommon *app);

      private:
        [[nodiscard]] auto buildOptionsList() -> std::list<Option> override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        [[nodiscard]] auto setTimeZoneIndex() -> unsigned int;
        [[nodiscard]] auto extractTimeZoneName(const std::string &name) const noexcept -> std::string;

        [[nodiscard]] static auto getTimeZones() -> std::vector<std::string>;
        [[nodiscard]] static auto sortTimeZones(std::vector<std::string> &&timeZones) -> std::vector<std::string>;

        const std::vector<std::string> timeZonesList;
        std::string selectedTimeZone;
    };
} // namespace gui
