// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewApnWindow.hpp"

#include <application-settings/widgets/SettingsStyle.hpp>
#include <application-settings/windows/WindowNames.hpp>

namespace gui
{

    NewApnWindow::NewApnWindow(app::ApplicationCommon *app)
        : AppWindow(app, gui::window::name::new_apn),
          apn(std::make_shared<packet_data::APN::Config>()), newApnModel{std::make_shared<NewApnModel>(app)}
    {
        buildInterface();
    }

    void NewApnWindow::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void NewApnWindow::buildInterface()
    {
        AppWindow::buildInterface();

        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::save));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        setTitle(utils::translate("app_settings_new_edit_apn"));

        list = new gui::ListView(this,
                                 style::settings::window::newApn::x,
                                 style::settings::window::newApn::y,
                                 style::settings::window::newApn::w,
                                 style::settings::window::newApn::h,
                                 newApnModel);
        setFocusItem(list);
        apnSettingsModel = new ApnSettingsModel(application);
    }

    void NewApnWindow::destroyInterface()
    {
        erase();
    }

    void NewApnWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode != ShowMode::GUI_SHOW_RETURN) {
            newApnModel->clearData();
        }

        if (mode == ShowMode::GUI_SHOW_INIT) {
            list->rebuildList();
        }

        if (apn != nullptr) {
            newApnModel->loadData(apn);
        }
    }

    auto NewApnWindow::handleSwitchData(SwitchData *data) -> bool
    {
        setSaveButtonVisible(false);

        if (data == nullptr) {
            return false;
        }

        auto *item = dynamic_cast<ApnItemData *>(data);
        if (item == nullptr) {
            return false;
        }

        auto apnItem = item->getApn();
        if (apnItem != nullptr) {
            apn = apnItem;
        }

        if (!apn->apn.empty()) {
            setSaveButtonVisible(true);
        }

        return true;
    }

    void NewApnWindow::setSaveButtonVisible(bool visible)
    {
        bottomBar->setActive(BottomBar::Side::CENTER, visible);
    }

    auto NewApnWindow::onInput(const InputEvent &inputEvent) -> bool
    {
        if (!inputEvent.isShortRelease()) {
            return false;
        }
        if (inputEvent.is(gui::KeyCode::KEY_ENTER)) {
            newApnModel->saveData(apn);
            LOG_DEBUG("APN: \"%s\" ", apn->apn.c_str());
            if (apn != nullptr && !apn->apn.empty()) {
                apnSettingsModel->saveAPN(apn);
                LOG_INFO("APN record saved: \"%s\" ", apn->apn.c_str());
                application->returnToPreviousWindow();
            }
            else {
                LOG_WARN("APN not saved, name is empty!");
            }
            return true;
        }

        return AppWindow::onInput(inputEvent);
    }
} // namespace gui
