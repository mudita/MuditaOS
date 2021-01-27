// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"
#include "application-settings-new/widgets/QuoteWidget.hpp"
#include "application-settings-new/models/QuotesModel.hpp"

#include <purefs/filesystem_paths.hpp>
#include <module-gui/gui/widgets/ListView.hpp>
#include <module-apps/InternalModel.hpp>
#include <utility>
#include <vfs.hpp>

namespace gui
{
    class QuotesMainWindow : public AppWindow
    {
      public:
        QuotesMainWindow(app::Application *app, std::shared_ptr<app::QuotesModel> model);

      private:
        void buildInterface() override;
        auto onInput(const InputEvent &inputEvent) -> bool override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        std::shared_ptr<app::QuotesModel> quotesModel = nullptr;
        gui::ListView *list                           = nullptr;
    };

} // namespace gui
