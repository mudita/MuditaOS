// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BaseSettingsWindow.hpp"

namespace gui
{
    BaseSettingsWindow::BaseSettingsWindow(app::ApplicationCommon *app, std::string name) : OptionWindow(app, name)
    {}

    void BaseSettingsWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void BaseSettingsWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        refreshOptionsList();
    }

    void BaseSettingsWindow::rebuildOptionList()
    {
        changeOptions(buildOptionsList());
    }

    void BaseSettingsWindow::refreshOptionsList()
    {
        refreshOptions(buildOptionsList());
    }

    void BaseSettingsWindow::refreshOptionsList(unsigned int pageIndex)
    {
        refreshOptions(buildOptionsList(), pageIndex);
    }
} // namespace gui
