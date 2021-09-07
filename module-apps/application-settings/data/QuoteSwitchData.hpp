// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/SwitchData.hpp>
#include <service-db/QuotesMessages.hpp>

namespace Quotes
{
    class QuotesModel;
};

namespace gui
{
    enum class QuoteAction
    {
        None,
        Add,
        Edit
    };

    class QuoteSwitchData : public gui::SwitchData
    {
      public:
        QuoteSwitchData(QuoteAction action, Quotes::QuoteRecord quote = {}) : action(action), quote(std::move(quote))
        {}

        [[nodiscard]] auto getQuote() const
        {
            return quote;
        }
        [[nodiscard]] auto getAction() const
        {
            return action;
        }

      private:
        QuoteAction action;
        Quotes::QuoteRecord quote;
    };
} // namespace gui
