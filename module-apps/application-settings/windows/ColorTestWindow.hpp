// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <functional>

#include "module-apps/application-settings/models/ColorTestModel.hpp"
#include "AppWindow.hpp"
#include "application-settings/widgets/ColorTestListView.hpp"

#include <gui/widgets/Label.hpp>
#include <gui/widgets/Image.hpp>
#include <gui/widgets/Window.hpp>
#include <gui/widgets/BottomBar.hpp>

namespace gui
{
    namespace window::name
    {
        inline constexpr auto color_test_window = "ColorTestWindow";
    } // namespace window::name

    class ColorTestWindow : public AppWindow
    {
      private:
        std::shared_ptr<ColorTestModel> colorTestModel = nullptr;
        gui::ColorTestListView *colorListView          = nullptr;

      public:
        explicit ColorTestWindow(app::Application *app);

        void applyInputCallback();
        void buildInterface() override;
        void setGlobalColorScheme(const ColorScheme &scheme);
        void resetGlobalColorScheme();
    };
} /* namespace gui */
