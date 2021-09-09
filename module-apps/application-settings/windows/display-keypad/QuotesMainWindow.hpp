// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/models/display-keypad/QuotesModel.hpp>
#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace gui
{
    class QuotesMainWindow : public AppWindow
    {
      public:
        explicit QuotesMainWindow(app::ApplicationCommon *app);

      private:
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        auto onInput(const InputEvent &inputEvent) -> bool override;

        std::shared_ptr<Quotes::QuotesModel> quotesModel = nullptr;
        gui::ListView *list                              = nullptr;
    };

} // namespace gui
