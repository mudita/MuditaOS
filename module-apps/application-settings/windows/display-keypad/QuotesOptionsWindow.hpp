// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/widgets/display-keypad/QuoteWidget.hpp>
#include <application-settings/windows/BaseSettingsWindow.hpp>

namespace gui
{
    class QuotesOptionsWindow : public BaseSettingsWindow
    {
      public:
        explicit QuotesOptionsWindow(app::ApplicationCommon *app);

      private:
        std::list<Option> buildOptionsList() override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        bool showDeleteNotification(std::string titleText);

        std::shared_ptr<Quotes::QuotesModel> quotesModel;
        Quotes::QuoteRecord quote;
        OptionWindowDestroyer rai_destroyer = OptionWindowDestroyer(*this);
    };
} // namespace gui
