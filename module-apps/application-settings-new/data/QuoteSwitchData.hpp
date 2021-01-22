// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-settings-new/models/QuotesModel.hpp"

#include <SwitchData.hpp>
#include <json/json11.hpp>
#include <utility>

namespace app
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
        QuoteSwitchData(QuoteAction action, app::QuoteRecord quote = {}) : action(action), quote(std::move(quote))
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
        app::QuoteRecord quote;
    };
} // namespace gui
