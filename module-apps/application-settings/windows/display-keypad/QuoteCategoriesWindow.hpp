// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/display-keypad/CategoriesModel.hpp>
#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace gui
{
    class QuoteCategoriesWindow : public AppWindow
    {
      public:
        QuoteCategoriesWindow(app::Application *app);

      private:
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        std::shared_ptr<Quotes::CategoriesModel> categoriesModel = nullptr;
        gui::ListView *list                                      = nullptr;
    };

} // namespace gui
