// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/advanced/ColorTestModel.hpp>
#include <application-settings/widgets/advanced/ColorTestListView.hpp>

namespace gui
{
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
