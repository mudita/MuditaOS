// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BellSettingsLanguageWindow.hpp"
#include <common/options/OptionBellMenu.hpp>

namespace gui
{
    BellSettingsLanguageWindow::BellSettingsLanguageWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::bell_settings::LanguageWindowPresenter::Presenter> &&presenter,
        const std::string &name)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        this->presenter->attach(this);
        buildInterface();
    }

    void BellSettingsLanguageWindow::buildInterface()
    {
        AppWindow::buildInterface();

        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        body = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height, true);

        auto topMessage = new TextFixedSize(body->firstBox);
        topMessage->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        topMessage->setFont(style::window::font::largelight);
        topMessage->setEdges(gui::RectangleEdge::None);
        topMessage->activeItem = false;
        topMessage->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        topMessage->setText(utils::translate("app_bell_settings_advanced_language"));
        topMessage->drawUnderline(false);

        auto data = presenter->getLanguages();
        spinner   = new UTF8Spinner({data.begin(), data.end()}, Boundaries::Fixed);
        spinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::center_layout_h);
        spinner->setFont(style::window::font::large);
        spinner->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        spinner->setFocusEdges(RectangleEdge::None);
        spinner->setCurrentValue(presenter->getSelectedLanguage());
        spinner->onValueChanged = [this](const auto &) {
            body->setMinMaxArrowsVisibility(spinner->isAtMin(), spinner->isAtMax());
        };
        body->getCenterBox()->addWidget(spinner);
        body->setMinMaxArrowsVisibility(spinner->isAtMin(), spinner->isAtMax());

        setFocusItem(spinner);
        body->resize();
    }

    bool BellSettingsLanguageWindow::onInput(const InputEvent &inputEvent)
    {
        if (spinner->onInput(inputEvent)) {
            return true;
        }
        else if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            presenter->setLanguage(spinner->getCurrentValue());
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

} // namespace gui
