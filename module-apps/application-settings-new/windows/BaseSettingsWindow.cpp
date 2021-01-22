// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BaseSettingsWindow.hpp"
#include <i18n/i18n.hpp>

namespace gui
{

    BaseSettingsWindow::BaseSettingsWindow(app::Application *app, std::string name) : OptionWindow(app, name)
    {}

    void BaseSettingsWindow::buildInterface()
    {
        OptionWindow::buildInterface();

        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
    }

    void BaseSettingsWindow::destroyInterface()
    {
        erase();
        invalidate();
    }

    void BaseSettingsWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void BaseSettingsWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        rebuildOptionList();
    }

    void BaseSettingsWindow::rebuildOptionList()
    {
        clearOptions();
        addOptions(buildOptionsList());
    }

} // namespace gui
