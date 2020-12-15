// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MmiPushWindow.hpp"
#include "MmiInternalMsgWindow.hpp"
#include <service-appmgr/model/ApplicationManager.hpp>
#include <service-appmgr/data/MmiActionsParams.hpp>
#include <i18n/i18n.hpp>

using namespace gui;

MmiInternalMsgWindow::MmiInternalMsgWindow(app::Application *app, const std::string &name) : MmiPushWindow(app, name)
{}

void MmiInternalMsgWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (auto metadata = dynamic_cast<app::manager::actions::MMIResultParams *>(data); metadata != nullptr) {
        auto result = metadata->getData();
        switch (result) {
        case app::manager::actions::MMIResultParams::MMIResult::Success:
            text->setText(utils::localize.get("app_desktop_info_mmi_result_success"));
            break;
        case app::manager::actions::MMIResultParams::MMIResult::Failed:
            text->setText(utils::localize.get("app_desktop_info_mmi_result_failed"));
            break;
        default:
            text->clear();
            break;
        }
    }
}
