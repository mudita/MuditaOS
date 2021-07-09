// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"

#include <Text.hpp>

namespace app::settingsInterface
{
    class DndSettings;
} // namespace app::settingsInterface

namespace gui
{

    namespace window
    {
        inline constexpr auto do_not_disturb_window = "DoNotDisturb";
    };

    class DoNotDisturbWindow : public BaseSettingsWindow
    {
      public:
        DoNotDisturbWindow(app::Application *app, app::settingsInterface::DndSettings *dndSettings);
        void buildInterface() override;

      private:
        app::settingsInterface::DndSettings *dndSettings = nullptr;
        Rect *bar                                        = nullptr;
        Text *descriptionText                            = nullptr;
        auto buildOptionsList() -> std::list<Option> override;
    };
} // namespace gui
