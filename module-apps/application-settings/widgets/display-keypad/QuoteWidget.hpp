// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/data/QuoteSwitchData.hpp>

#include <ImageBox.hpp>
#include <Label.hpp>
#include <ListItem.hpp>

namespace gui
{
    class QuoteWidget : public ListItem
    {
      public:
        QuoteWidget(const Quotes::QuoteRecord &quoteRecord,
                    std::function<void(bool)> enableQuoteCallback,
                    std::function<void(const UTF8 &text)> navBarTemporaryMode = nullptr,
                    std::function<void()> navBarRestoreFromTemporaryMode      = nullptr);

        [[nodiscard]] auto getQuoteData() const -> Quotes::QuoteRecord
        {
            return quote;
        }

      private:
        HBox *hBox              = nullptr;
        Label *descriptionLabel = nullptr;
        ImageBox *tickImage     = nullptr;

        Quotes::QuoteRecord quote;
        std::function<void(bool)> enableQuote                     = nullptr;
        std::function<void(const UTF8 &text)> navBarTemporaryMode = nullptr;
        std::function<void()> navBarRestoreFromTemporaryMode      = nullptr;
    };

} /* namespace gui */
