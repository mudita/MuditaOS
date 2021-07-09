// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"
#include "application-settings/widgets/QuoteWidget.hpp"
#include "application-settings/models/QuotesModel.hpp"

#include <purefs/filesystem_paths.hpp>
#include <module-gui/gui/widgets/ListView.hpp>
#include <apps-common/InternalModel.hpp>
#include <utility>

namespace gui
{
    class QuotesMainWindow : public AppWindow
    {
      public:
        explicit QuotesMainWindow(app::Application *app);

      private:
        void buildInterface() override;
        void onBeforeShow(ShowMode mode, SwitchData *data);
        auto onInput(const InputEvent &inputEvent) -> bool override;

        std::shared_ptr<Quotes::QuotesModel> quotesModel = nullptr;
        gui::ListView *list                              = nullptr;
    };

} // namespace gui
