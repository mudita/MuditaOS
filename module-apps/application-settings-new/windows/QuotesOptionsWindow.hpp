// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"
#include "QuotesMainWindow.hpp"
#include "application-settings-new/widgets/QuoteWidget.hpp"

namespace gui
{
    class QuotesOptionsWindow : public BaseSettingsWindow
    {
      public:
        QuotesOptionsWindow(app::Application *app, std::shared_ptr<app::QuotesModel> model);

      private:
        std::list<Option> buildOptionsList() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        std::shared_ptr<app::QuotesModel> quotesModel;
        app::QuoteRecord quote;
    };
} // namespace gui
