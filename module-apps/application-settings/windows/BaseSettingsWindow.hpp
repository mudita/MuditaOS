// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <service-db/Settings.hpp>
#include <windows/OptionWindow.hpp>

namespace gui
{
    class BaseSettingsWindow : public OptionWindow
    {
      public:
        BaseSettingsWindow(app::Application *app, std::string name);

        void rebuild() override;

        void rebuildOptionList();
        void refreshOptionsList();
        void refreshOptionsList(unsigned int pageIndex);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      protected:
        virtual auto buildOptionsList() -> std::list<gui::Option> = 0;
    };
} // namespace gui
