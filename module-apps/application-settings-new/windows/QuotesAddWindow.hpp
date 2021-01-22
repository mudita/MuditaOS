// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseSettingsWindow.hpp"
#include "QuotesMainWindow.hpp"
#include "application-settings-new/models/QuotesModel.hpp"

namespace gui
{
    class CheckBoxWithLabel;

    class QuoteAddEditWindow : public AppWindow
    {
      public:
        QuoteAddEditWindow(app::Application *app, std::shared_ptr<app::QuotesModel> model);
        void buildInterface() override;

      private:
        auto onInput(const InputEvent &inputEvent) -> bool override;
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void setAuthorCharactersCount(uint32_t count);
        void setQuoteCharactersCount(uint32_t count);

        gui::Text *quoteText          = nullptr;
        gui::Text *authorText         = nullptr;
        gui::Label *authorCharCounter = nullptr;
        gui::Label *quoteCharCounter  = nullptr;

        QuoteAction quoteAction;
        app::QuoteRecord quoteData;
        std::shared_ptr<app::QuotesModel> quoteModel;
    };
} // namespace gui
