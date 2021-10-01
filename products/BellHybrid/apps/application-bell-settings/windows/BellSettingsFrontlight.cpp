// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "application-bell-settings/ApplicationBellSettings.hpp"
#include "BellSettingsStyle.hpp"
#include "BellSettingsFrontlight.hpp"

#include <gui/input/InputEvent.hpp>
#include <apps-common/options/OptionStyle.hpp>
#include <common/windows/BellFinishedWindow.hpp>
#include <widgets/SideListView.hpp>
#include <widgets/BellBaseLayout.hpp>

namespace gui
{
    BellSettingsFrontlightWindow::BellSettingsFrontlightWindow(
        app::ApplicationCommon *app,
        std::unique_ptr<app::bell_settings::FrontlightWindowContract::Presenter> &&windowPresenter,
        std::string name)
        : AppWindow(app, std::move(name)), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void BellSettingsFrontlightWindow::rebuild()
    {
        erase();
        buildInterface();
    }

    void BellSettingsFrontlightWindow::buildInterface()
    {
        AppWindow::buildInterface();
        statusBar->setVisible(false);
        header->setTitleVisibility(true);
        bottomBar->setVisible(false);

        presenter->createData();

        body = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height);

        topText = new Label(body->firstBox);
        topText->setMinimumSize(style::bell_base_layout::w, style::bell_base_layout::outer_layouts_h);
        topText->setText(utils::translate("app_bell_settings_frontlight_top_message"));
        topText->setFont(bell_settings_style::top_text::font);
        topText->setEdges(RectangleEdge::None);
        topText->activeItem = false;
        topText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));

        body->getCenterBox()->addWidget(presenter->getPagesProvider()->getSpinner());

        body->resize();

        presenter->loadData();

        setFocusItem(body);
    }

    bool BellSettingsFrontlightWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (body->onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER)) {
            presenter->saveData();

            application->switchWindow(BellFinishedWindow::defaultName,
                                      BellFinishedWindowData::Factory::create(
                                          "big_check_W_M",
                                          utils::translate("app_bell_settings_frontlight_finished_message"),
                                          window::name::bellSettingsAdvanced));
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }
} /* namespace gui */
