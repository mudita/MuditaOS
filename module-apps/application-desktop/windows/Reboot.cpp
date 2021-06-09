// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Reboot.hpp"
#include "../ApplicationDesktop.hpp"
#include <Style.hpp>
#include <i18n/i18n.hpp>

namespace gui
{

    RebootWindow::RebootWindow(app::Application *app, std::unique_ptr<PowerOffPresenter> &&presenter)
        : AppWindow(app, app::window::name::desktop_reboot), presenter(std::move(presenter))
    {
        buildInterface();
    }

    void RebootWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void RebootWindow::buildInterface()
    {
        AppWindow::buildInterface();

        auto text_y_offset = 270;
        auto text_height   = 300;

        text = new Text(
            this,
            style::window::default_left_margin,
            text_y_offset,
            style::window_width - style::window::default_left_margin * 2,
            text_height,
            "Phone have to reboot, press any key to confirm and remove battery for 10 sec to perform full reboot");
        text->setFilled(false);
        text->setBorderColor(gui::ColorFullBlack);
        text->setFont(style::header::title_font);
        text->setEdges(RectangleEdge::None);
        text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Bottom));
    }

    void RebootWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void RebootWindow::invalidate() noexcept
    {
        text = nullptr;
    }

    void RebootWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
    }

    bool RebootWindow::onInput(const InputEvent &inputEvent)
    {
        presenter->powerOff();
        return true;
    }

} /* namespace gui */
