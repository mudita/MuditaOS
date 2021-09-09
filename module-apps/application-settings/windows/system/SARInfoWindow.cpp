// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SARInfoWindow.hpp"

#include <application-settings/windows/WindowNames.hpp>
#include <application-settings/widgets/SettingsStyle.hpp>

#include <i18n/i18n.hpp>

namespace gui
{
    SARInfoWindow::SARInfoWindow(app::ApplicationCommon *app,
                                 std::unique_ptr<SARInfoWindowContract::Presenter> &&windowPresenter)
        : gui::AppWindow(app, window::name::sar), presenter{std::move(windowPresenter)}
    {
        presenter->attach(this);
        buildInterface();
    }

    void SARInfoWindow::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("app_settings_sar"));

        namespace sarStyle = style::settings::window::sar;
        sarInfoText =
            new gui::Text(this, sarStyle::LeftMargin, sarStyle::TopMargin, sarStyle::Width, sarStyle::text::Height);
        sarInfoText->setEdges(gui::RectangleEdge::None);
        sarInfoText->setFont(::style::window::font::medium);
        sarInfoText->setAlignment(gui::Alignment{gui::Alignment::Vertical::Top});
        sarInfoText->setPenFocusWidth(::style::window::default_border_focus_w);
        sarInfoText->setPenWidth(::style::window::default_border_rect_no_focus);
        sarInfoText->setEditMode(gui::EditMode::Scroll);
        sarInfoText->setCursorStartPosition(gui::CursorStartPosition::DocumentBegin);

        bottomBar->setActive(gui::BottomBar::Side::RIGHT, true);
        bottomBar->setText(gui::BottomBar::Side::RIGHT, utils::translate(::style::strings::common::back));

        setFocusItem(sarInfoText);
    }

    void SARInfoWindow::onBeforeShow(gui::ShowMode /*mode*/, gui::SwitchData * /*data*/)
    {
        sarInfoText->setRichText(presenter->getSarInfo());
    }
} // namespace gui
