// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "QuotesAddWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>

#include <widgets/text/Text.hpp>

namespace style
{
    constexpr auto counterWidth = 70;
    constexpr auto headerWidth  = style::window::default_body_width - counterWidth;
} // namespace style

namespace gui
{
    namespace
    {
        constexpr auto maxQuoteCharactersCount  = 150U;
        constexpr auto maxQuoteLinesCount       = 4;
        constexpr auto maxAuthorCharactersCount = 30U;

        auto formatCounterText(uint32_t counter, uint32_t maxValue) -> std::string
        {
            std::ostringstream counterText;
            counterText << counter << '/' << maxValue;
            return counterText.str();
        }
    } // namespace

    QuoteAddEditWindow::QuoteAddEditWindow(app::ApplicationCommon *app)
        : AppWindow(app, gui::window::name::quotes), quoteModel(std::make_shared<Quotes::QuotesModel>(app))
    {
        buildInterface();
    }

    void QuoteAddEditWindow::buildInterface()
    {
        AppWindow::buildInterface();

        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::save));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        auto vBox = new VBox(this,
                             style::window::default_left_margin,
                             style::window::default_vertical_pos + style::margins::very_big,
                             style::window::default_body_width,
                             style::window::default_body_height);

        vBox->setEdges(RectangleEdge::None);
        vBox->setPenFocusWidth(::style::window::default_border_focus_w);
        vBox->setPenWidth(::style::window::default_border_rect_no_focus);

        auto authorHeader = new HBox(vBox, 0, 0, 0, 0);
        authorHeader->setMinimumSize(style::window::default_body_width, style::window::label::default_h);
        authorHeader->setEdges(gui::RectangleEdge::None);
        authorHeader->activeItem = false;

        auto authorLabel = new Label(authorHeader, 0, 0, 0, 0);
        authorLabel->setMinimumSize(style::headerWidth, style::window::label::default_h);
        authorLabel->setEdges(RectangleEdge::None);
        authorLabel->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left});
        authorLabel->setText(utils::translate("app_settings_display_wallpaper_quotes_author"));
        authorLabel->setPenFocusWidth(::style::window::default_border_focus_w);
        authorLabel->setFont(::style::window::font::verysmall);
        authorLabel->setPadding(gui::Padding(0, 0, 0, 0));
        authorLabel->activeItem = false;

        authorText = new gui::Text(vBox, 0, 0, 0, 0);
        authorText->setMinimumSize(style::window::default_body_width, style::window::label::default_h);
        authorText->setAlignment(gui::Alignment{gui::Alignment::Vertical::Top});
        authorText->setPenFocusWidth(::style::window::default_border_focus_w);
        authorText->setPenWidth(::style::window::default_border_rect_no_focus);
        authorText->setEdges(gui::RectangleEdge::Bottom);
        authorText->setFont(::style::window::font::medium);
        authorText->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &text) { navBarTemporaryMode(text); },
            [=]() { navBarRestoreFromTemporaryMode(); },
            [=]() { selectSpecialCharacter(); }));
        authorText->setTextLimitType(gui::TextLimitType::MaxSignsCount, maxAuthorCharactersCount);

        auto quoteHeader = new HBox(vBox, 0, 0, 0, 0);
        quoteHeader->setMinimumSize(style::window::default_body_width, style::window::label::default_h);
        quoteHeader->activeItem = false;
        quoteHeader->setEdges(gui::RectangleEdge::None);

        auto quoteLabel = new Label(quoteHeader, 0, 0, 0, 0);
        quoteLabel->setMinimumSize(style::headerWidth, style::window::label::default_h);
        quoteLabel->setEdges(RectangleEdge::None);
        quoteLabel->setPenFocusWidth(::style::window::default_border_focus_w);
        quoteLabel->setText(utils::translate("app_settings_display_wallpaper_quotes_note"));
        quoteLabel->setFont(::style::window::font::verysmall);
        quoteLabel->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left});
        quoteLabel->activeItem = false;

        quoteText = new gui::Text(vBox, 0, 0, 0, 0);
        quoteText->setAlignment(gui::Alignment{gui::Alignment::Vertical::Top});
        quoteText->setMinimumSize(style::window::default_body_width,
                                  style::window::label::default_h * maxQuoteLinesCount);
        quoteText->setPenFocusWidth(::style::window::default_border_focus_w);
        quoteText->setPenWidth(::style::window::default_border_rect_no_focus);
        quoteText->setEdges(gui::RectangleEdge::Bottom);
        quoteText->setFont(::style::window::font::medium);
        quoteText->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &text) { navBarTemporaryMode(text); },
            [=]() { navBarRestoreFromTemporaryMode(); },
            [=]() { selectSpecialCharacter(); }));
        quoteText->setTextLimitType(gui::TextLimitType::MaxSignsCount, maxQuoteCharactersCount);
        quoteText->setTextChangedCallback([this](Item &, const UTF8 &text) { setQuoteCharactersCount(text.length()); });

        quoteCharCounter = new gui::Label(vBox, 0, 0, 0, 0);
        quoteCharCounter->setMinimumSize(style::counterWidth, style::window::label::default_h);
        quoteCharCounter->setEdges(gui::RectangleEdge::None);
        quoteCharCounter->setFont(::style::window::font::verysmall);
        quoteCharCounter->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Right});

        setTitle(utils::translate("app_settings_display_wallpaper_quotes_new"));
        vBox->resizeItems();
        setFocusItem(quoteText);
    }

    void QuoteAddEditWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        auto *quotedata = dynamic_cast<QuoteSwitchData *>(data);
        if (quotedata == nullptr) {
            return;
        }

        quoteAction = quotedata->getAction();
        quoteData   = quotedata->getQuote();

        if (quoteAction == QuoteAction::Edit) {
            setTitle(utils::translate("app_settings_display_wallpaper_quotes_edit"));
            quoteText->setText(quoteData.quote);
            authorText->setText(quoteData.author);
        }
        else {
            setTitle(utils::translate("app_settings_display_wallpaper_quotes_new"));
        }

        setQuoteCharactersCount(quoteText->getText().length());
    }

    bool QuoteAddEditWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            LOG_DEBUG("Save Quote requested");
            quoteData.quote  = quoteText->getText();
            quoteData.author = authorText->getText();

            if (quoteAction == QuoteAction::Add) {
                quoteModel->add(quoteData);
            }
            else {
                quoteModel->edit(quoteData);
            }

            if (quoteAction != QuoteAction::Add) {
                application->switchWindow(gui::name::window::main_window);
            }
            application->returnToPreviousWindow();
        }

        return AppWindow::onInput(inputEvent);
    }

    void QuoteAddEditWindow::setQuoteCharactersCount(std::uint32_t count)
    {
        quoteCharCounter->setText(formatCounterText(count, maxQuoteCharactersCount));
    }

} // namespace gui
