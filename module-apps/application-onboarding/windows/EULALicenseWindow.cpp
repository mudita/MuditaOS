// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EULALicenseWindow.hpp"

#include <i18n/i18n.hpp>
#include <Style.hpp>

#include <module-apps/application-notes/style/NotePreviewStyle.hpp>
#include <module-apps/application-onboarding/ApplicationOnBoarding.hpp>
#include <module-apps/application-onboarding/data/OnBoardingSwitchData.hpp>

namespace app::onBoarding
{
    EULALicenseWindow::EULALicenseWindow(app::Application *app,
                                         std::unique_ptr<EULALicenseWindowContract::Presenter> &&windowPresenter)
        : gui::AppWindow(app, gui::window::name::onBoarding_eula), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    EULALicenseWindow::~EULALicenseWindow() noexcept
    {
        destroyInterface();
    }

    void EULALicenseWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void EULALicenseWindow::destroyInterface()
    {
        erase();
    }

    void EULALicenseWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_onboarding_eula_license"));

        namespace previewStyle = app::notes::style::preview;
        eulaText               = new gui::Text(
            this, previewStyle::LeftMargin, previewStyle::TopMargin, previewStyle::Width, previewStyle::text::Height);
        eulaText->setEdges(gui::RectangleEdge::None);
        eulaText->setFont(::style::window::font::medium);
        eulaText->setAlignment(gui::Alignment{gui::Alignment::Vertical::Top});
        eulaText->setPenFocusWidth(::style::window::default_border_focus_w);
        eulaText->setPenWidth(::style::window::default_border_rect_no_focus);
        eulaText->setEditMode(gui::EditMode::Scroll);
        eulaText->setCursorStartPosition(gui::CursorStartPosition::DocumentBegin);

        bottomBar->setActive(gui::BottomBar::Side::LEFT, true);

        bottomBar->setActive(gui::BottomBar::Side::CENTER, true);
        bottomBar->setText(gui::BottomBar::Side::CENTER, utils::translate(::style::strings::common::accept));

        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(::style::strings::common::back));

        setFocusItem(eulaText);
    }

    gui::status_bar::Configuration EULALicenseWindow::configureStatusBar(
        gui::status_bar::Configuration appConfiguration)
    {
        appConfiguration.setIndicator(gui::status_bar::Indicator::SimCard, false);
        return appConfiguration;
    }

    void EULALicenseWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        eulaText->setRichText(presenter->getEULA());
    }

    bool EULALicenseWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            presenter->acceptEULA();

            application->switchWindow(gui::window::name::onBoarding_start_configuration,
                                      gui::ShowMode::GUI_SHOW_INIT,
                                      std::make_unique<OnBoardingSwitchData>());
        }
        return AppWindow::onInput(inputEvent);
    }
} // namespace app::onBoarding
