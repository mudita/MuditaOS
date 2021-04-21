// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ActiveNotificationsModel.hpp"
#include <module-db/queries/notifications/QueryNotificationsClear.hpp>
#include <service-appmgr/Controller.hpp>
#include <application-messages/ApplicationMessages.hpp>
#include <application-call/data/CallSwitchData.hpp>
#include <SystemManager/messages/TetheringStateRequest.hpp>

using namespace gui;

void ActiveNotificationsModel::setParentBottomBar(const UTF8 &left, const UTF8 &center, const UTF8 &right)
{
    parent->setBottomBarText(left, BottomBar::Side::LEFT);
    parent->setBottomBarText(center, BottomBar::Side::CENTER);
    parent->setBottomBarText(right, BottomBar::Side::RIGHT);
}
auto ActiveNotificationsModel::create(const notifications::NotSeenSMSNotification *notification)
    -> NotificationListItem *
{
    auto item                  = NotificationsModel::create(notification);
    item->focusChangedCallback = [this](gui::Item &_item) {
        if (_item.focus) {
            setParentBottomBar({}, utils::translate("app_desktop_show"), utils::translate("app_desktop_clear"));
            return true;
        }
        return false;
    };
    item->activatedCallback = [this]([[maybe_unused]] gui::Item &_item) {
        return app::manager::Controller::sendAction(parent->getApplication(),
                                                    app::manager::actions::Launch,
                                                    std::make_unique<app::ApplicationLaunchData>(app::name_messages));
    };
    item->inputCallback = [this]([[maybe_unused]] Item &item, const InputEvent &inputEvent) {
        if (inputEvent.isShortPress()) {
            if (inputEvent.is(KeyCode::KEY_RF)) {
                DBServiceAPI::GetQuery(
                    parent->getApplication(),
                    db::Interface::Name::Notifications,
                    std::make_unique<db::query::notifications::Clear>(NotificationsRecord::Key::Sms));
                return true;
            }
        }
        return false;
    };
    item->setDismissible(true);
    return item;
}
auto ActiveNotificationsModel::create(const notifications::NotSeenCallNotification *notification)
    -> NotificationListItem *
{
    auto item               = NotificationsModel::create(notification);
    item->activatedCallback = [this]([[maybe_unused]] gui::Item &_item) {
        return app::manager::Controller::sendAction(parent->getApplication(), app::manager::actions::ShowCallLog);
    };

    std::function<void()> onRightFunctionalCallback = [this]() {
        DBServiceAPI::GetQuery(parent->getApplication(),
                               db::Interface::Name::Notifications,
                               std::make_unique<db::query::notifications::Clear>(NotificationsRecord::Key::Calls));
    };
    std::function<void()> onKeyLeftFunctionalCallback = nullptr;

    if (notification->hasRecord()) {
        if (const auto &record = notification->getRecord(); !record->numbers.empty()) {
            onKeyLeftFunctionalCallback = [this, number = record->numbers[0].number]() {
                app::manager::Controller::sendAction(parent->getApplication(),
                                                     app::manager::actions::Dial,
                                                     std::make_unique<app::ExecuteCallData>(number));
            };
        }
    }
    item->inputCallback = [keyRightFunctionalCb = std::move(onRightFunctionalCallback),
                           keyLeftFunctionalCb  = std::move(onKeyLeftFunctionalCallback)]([[maybe_unused]] Item &item,
                                                                                         const InputEvent &inputEvent) {
        if (inputEvent.isShortPress()) {
            if (inputEvent.is(KeyCode::KEY_RF)) {
                keyRightFunctionalCb();
                return true;
            }
            else if (inputEvent.is(KeyCode::KEY_LF) && keyLeftFunctionalCb != nullptr) {
                keyLeftFunctionalCb();
                return true;
            }
        }
        return false;
    };

    item->focusChangedCallback = [this, canCall = notification->hasRecord()](gui::Item &_item) {
        if (_item.focus) {
            UTF8 bottomBarLeftText = canCall ? UTF8{utils::translate("common_call")} : UTF8{};
            setParentBottomBar(
                bottomBarLeftText, utils::translate("app_desktop_show"), utils::translate("app_desktop_clear"));
        }
        return true;
    };

    item->setDismissible(true);
    return item;
}

auto ActiveNotificationsModel::create(const notifications::TetheringNotification *notification)
    -> NotificationListItem *
{
    auto item               = NotificationsModel::create(notification);
    item->activatedCallback = [this]([[maybe_unused]] gui::Item &_item) {
        return parent->getApplication()->bus.sendUnicast(
            std::make_shared<sys::TetheringStateRequest>(sys::phone_modes::Tethering::Off),
            service::name::system_manager);
    };
    item->focusChangedCallback = [this](gui::Item &_item) {
        if (_item.focus) {
            setParentBottomBar({}, utils::translate("common_disconnect"), {});
            return true;
        }
        return false;
    };
    return item;
}
