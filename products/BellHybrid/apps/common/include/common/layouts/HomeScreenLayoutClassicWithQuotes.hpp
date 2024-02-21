// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "HomeScreenLayoutClassic.hpp"

namespace gui
{
    class HomeScreenLayoutClassicWithQuotes : public HomeScreenLayoutClassic
    {
      public:
        explicit HomeScreenLayoutClassicWithQuotes(std::string name);
        void setQuoteText(const UTF8 &quoteContent, const UTF8 &quoteAuthor) override;

      protected:
        void buildInterface() override;

        VBox *textBox{nullptr};
        TextFixedSize *quotes{nullptr};
        TextFixedSize *author{nullptr};
        VBox *imgBox{nullptr};
        Image *quoteImg{nullptr};

      private:
        void showQuotes();
        void hideQuotes();
    };
}; // namespace gui
