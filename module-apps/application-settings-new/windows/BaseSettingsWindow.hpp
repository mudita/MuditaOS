// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <Application.hpp>
#include <module-apps/windows/OptionWindow.hpp>

namespace gui
{

    class BaseSettingsWindow : public OptionWindow
    {
      public:
        BaseSettingsWindow(app::Application *app, std::string name);

        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;

      protected:
        virtual void invalidate() noexcept {};
    };
} // namespace gui
