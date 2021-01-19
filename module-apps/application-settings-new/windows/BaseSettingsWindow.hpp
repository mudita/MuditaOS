// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include "windows/AppWindow.hpp"
#include "windows/OptionWindow.hpp"
#include <service-db/Settings.hpp>

namespace gui
{

    class BaseSettingsWindow : public OptionWindow
    {
      public:
        BaseSettingsWindow(app::Application *app, std::string name);

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

        void rebuildOptionList();
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

      protected:
        virtual void invalidate() noexcept {};
        virtual auto buildOptionsList() -> std::list<gui::Option> = 0;
    };
} // namespace gui
