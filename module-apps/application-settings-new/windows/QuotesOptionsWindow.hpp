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
        explicit QuotesOptionsWindow(app::Application *app);

      private:
        std::list<Option> buildOptionsList() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;

        std::shared_ptr<Quotes::QuotesModel> quotesModel;
        Quotes::QuoteRecord quote;
    };
} // namespace gui
