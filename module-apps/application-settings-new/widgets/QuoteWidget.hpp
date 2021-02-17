// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-settings-new/data/QuoteSwitchData.hpp"

#include <BoxLayout.hpp>
#include <Image.hpp>
#include <Label.hpp>
#include <ListItem.hpp>
#include <json/json11.hpp>

namespace gui
{
    class QuoteWidget : public ListItem
    {
      public:
        QuoteWidget(const app::QuoteRecord &quote,
                    std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                    std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);

        [[nodiscard]] auto getQuoteData() const -> app::QuoteRecord
        {
            return quote;
        }

      private:
        gui::HBox *hBox              = nullptr;
        gui::Label *inputBoxLabel    = nullptr;
        gui::Label *descriptionLabel = nullptr;
        gui::Image *tickImage        = nullptr;

        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;

        app::QuoteRecord quote;
    };

} /* namespace gui */
