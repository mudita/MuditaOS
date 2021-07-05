// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "MmiPushWindow.hpp"
#include "MmiInternalMsgWindow.hpp"
#include <service-appmgr/model/ApplicationManager.hpp>
#include <service-appmgr/data/MmiActionsParams.hpp>
#include <application-desktop/data/Mmi.hpp>
#include <i18n/i18n.hpp>

using namespace gui;

MmiInternalMsgWindow::MmiInternalMsgWindow(app::Application *app, const std::string &name) : MmiPushWindow(app, name)
{}

void MmiInternalMsgWindow::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (auto metadata = dynamic_cast<mmiactions::MMIResultParams *>(data); metadata != nullptr) {
        if (metadata->getCustomData() != nullptr) {
            mmi::MMIMessageVisitor customMMIvisitor;
            std::string displayMessage;
            metadata->getCustomData()->accept(customMMIvisitor, displayMessage);
            text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
            text->setText(displayMessage);
        }
        else {
            handleInternalMessages(metadata);
        }
    }
}

void MmiInternalMsgWindow::handleInternalMessages(mmiactions::MMIResultParams *metadata)
{
    std::string displayMessage;
    text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));
    auto result = metadata->getData();
    switch (result) {
    case mmiactions::MMIResultParams::MMIResult::Success:
        text->setText(displayMessage + utils::translate("app_desktop_info_mmi_result_success"));
        break;
    case mmiactions::MMIResultParams::MMIResult::Failed:
        text->setText(displayMessage + utils::translate("app_desktop_info_mmi_result_failed"));
        break;
    default:
        text->clear();
        break;
    }
}
