// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <common/options/BellOptionWithDescriptionWindow.hpp>

namespace gui
{
    class BellSettingsFactoryResetWindow : public BellOptionWithDescriptionWindow
    {
      public:
        static constexpr auto defaultName = "BellSettingsFactoryResetWindow";
        explicit BellSettingsFactoryResetWindow(app::ApplicationCommon *app, const std::string &name = defaultName);
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      private:
        std::list<Option> settingsOptionsList();
        const UTF8 resetStr;
        const UTF8 backStr;
    };
} /* namespace gui */
