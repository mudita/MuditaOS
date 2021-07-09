// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"
#include "QuotesMainWindow.hpp"
#include "application-settings/models/QuotesModel.hpp"

namespace gui
{
    class CheckBoxWithLabel;

    class QuoteAddEditWindow : public AppWindow
    {
      public:
        explicit QuoteAddEditWindow(app::Application *app);
        void buildInterface() override;

      private:
        auto onInput(const InputEvent &inputEvent) -> bool override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void setQuoteCharactersCount(uint32_t count);

        gui::Text *quoteText          = nullptr;
        gui::Text *authorText         = nullptr;
        gui::Label *authorCharCounter = nullptr;
        gui::Label *quoteCharCounter  = nullptr;

        QuoteAction quoteAction;
        Quotes::QuoteRecord quoteData;
        std::shared_ptr<Quotes::QuotesModel> quoteModel;
    };
} // namespace gui
