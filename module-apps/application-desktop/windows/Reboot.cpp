// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationDesktop.hpp"
#include "Reboot.hpp"

#include <i18n/i18n.hpp>
#include <Style.hpp>

namespace gui
{

    RebootWindow::RebootWindow(app::ApplicationCommon *app, std::unique_ptr<PowerOffPresenter> &&presenter)
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

        text = new Text(this,
                        style::window::default_left_margin,
                        text_y_offset,
                        style::window_width - style::window::default_left_margin * 2,
                        text_height);
        text->setText(utils::translate("phone_needs_rebooting"));
        text->setFilled(false);
        text->setBorderColor(gui::ColorFullBlack);
        text->setFont(style::header::font::title);
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
    {}

    bool RebootWindow::onInput(const InputEvent &inputEvent)
    {
        presenter->powerOff();
        return true;
    }

} /* namespace gui */
