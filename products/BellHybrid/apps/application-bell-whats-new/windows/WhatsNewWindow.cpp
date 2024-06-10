// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WhatsNewWindow.hpp"

#include <common/options/OptionBellMenu.hpp>
#include <common/data/BatteryStatusSwitchData.hpp>

namespace app::whatsNew
{
    using namespace gui;

    WhatsNewWindow::WhatsNewWindow(app::ApplicationCommon *app,
                                   std::unique_ptr<WhatsNewContract::Presenter> &&presenter,
                                   const std::string &name)
        : AppWindow(app, name), presenter{std::move(presenter)}
    {
        buildInterface();
    }

    void WhatsNewWindow::buildInterface()
    {
        AppWindow::buildInterface();
    }

    void WhatsNewWindow::onBeforeShow(gui::ShowMode mode, gui::SwitchData *data)
    {
        AppWindow::onBeforeShow(mode, data);
    }

    bool WhatsNewWindow::onInput(const gui::InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }
} // namespace app::whatsNew
