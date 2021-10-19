// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ActiveNotificationsModel.hpp"
#include "ApplicationDesktop.hpp"

#include <application-call/data/CallSwitchData.hpp>
#include <application-messages/Constants.hpp>
#include <application-messages/data/SMSdata.hpp>
#include <gsl/assert>
#include <module-db/queries/notifications/QueryNotificationsClear.hpp>
#include <queries/messages/threads/QueryThreadGetByNumber.hpp>
#include <service-appmgr/Constants.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-appmgr/messages/SwitchRequest.hpp>
#include <service-time/include/service-time/AlarmServiceAPI.hpp>
#include <service-time/AlarmMessage.hpp>
#include <service-time/Constants.hpp>
#include <apps-common/popups/data/AlarmPopupRequestParams.hpp>

using namespace gui;

namespace
{
    using Notification = const notifications::NotificationWithContact *;
    void setSMSFocusChangedCallback(NotificationListItem *item, ActiveNotificationsModel *model)
    {
        item->focusChangedCallback = [model](gui::Item &_item) {
            if (_item.focus) {
                model->setParentBottomBar(
                    {}, utils::translate("app_desktop_show"), utils::translate("app_desktop_clear"));
                return true;
            }
            return false;
        };
    }

    auto createSMSActivatedCallback(app::ApplicationCommon *app)
    {
        return [app]([[maybe_unused]] gui::Item &_item) {
            return app::manager::Controller::sendAction(
                app, app::manager::actions::Launch, std::make_unique<app::ApplicationLaunchData>(app::name_messages));
        };
    }

    auto createSMSActivatedCallback(app::ApplicationCommon *app, const ContactRecord &record)
    {
        Expects(not record.numbers.empty());
        return [app, number = record.numbers[0].number]([[maybe_unused]] gui::Item &_item) {
            auto query = std::make_unique<db::query::ThreadGetByNumber>(number);
            auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::SMSThread);

            auto queryCallback = [app](db::QueryResult *msg) -> bool {
                Expects(typeid(*msg) == typeid(db::query::ThreadGetByNumberResult));
                auto result  = static_cast<db::query::ThreadGetByNumberResult *>(msg);
                auto data    = std::make_unique<SMSThreadData>(std::make_shared<ThreadRecord>(result->getThread()));
                auto request = std::make_shared<app::manager::SwitchRequest>(
                    app->GetName(), app::name_messages, gui::name::window::thread_view, std::move(data));
                return app->bus.sendUnicast(std::move(request), service::name::appmgr);
            };
            task->setCallback(std::move(queryCallback));
            task->execute(app, static_cast<app::ApplicationDesktop *>(app));
            return true;
        };
    }

    void setSMSActivatedCallback(NotificationListItem *item, Notification provider, app::ApplicationCommon *app)
    {
        if (provider->hasRecord() && not provider->getRecord().numbers.empty()) {
            item->activatedCallback = createSMSActivatedCallback(app, provider->getRecord());
        }
        else {
            item->activatedCallback = createSMSActivatedCallback(app);
        }
    }

    void clearSMSNotifications(app::ApplicationCommon *app)
    {
        DBServiceAPI::GetQuery(app,
                               db::Interface::Name::Notifications,
                               std::make_unique<db::query::notifications::Clear>(NotificationsRecord::Key::Sms));
    }

    void setSMSOnInputCallback(NotificationListItem *item, app::ApplicationCommon *app)
    {
        item->inputCallback = [app]([[maybe_unused]] Item &item, const InputEvent &inputEvent) {
            if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
                clearSMSNotifications(app);
                return true;
            }
            return false;
        };
    }

    void setSMSDismissCallback(NotificationListItem *item, app::ApplicationCommon *app)
    {
        item->dismissCallback = [app]() { clearSMSNotifications(app); };
    }

    void setCallFocusChangedCallback(NotificationListItem *item, Notification provider, ActiveNotificationsModel *model)
    {
        item->focusChangedCallback = [model, canCall = provider->hasRecord()](gui::Item &_item) {
            if (_item.focus) {
                UTF8 bottomBarLeftText = canCall ? UTF8{utils::translate("common_call")} : UTF8{};
                model->setParentBottomBar(
                    bottomBarLeftText, utils::translate("app_desktop_show"), utils::translate("app_desktop_clear"));
            }
            return true;
        };
    }

    void setCallActivatedCallback(NotificationListItem *item, app::ApplicationCommon *app)
    {
        item->activatedCallback = [app]([[maybe_unused]] gui::Item &_item) {
            return app::manager::Controller::sendAction(app, app::manager::actions::ShowCallLog);
        };
    }

    auto createCallOnRightFunctionalCallback(app::ApplicationCommon *app) -> std::function<void()>
    {
        return [app]() {
            DBServiceAPI::GetQuery(app,
                                   db::Interface::Name::Notifications,
                                   std::make_unique<db::query::notifications::Clear>(NotificationsRecord::Key::Calls));
        };
    }
    auto createCallOnLeftFunctionalCallback(app::ApplicationCommon *app, Notification provider) -> std::function<void()>
    {
        if (!provider->hasRecord()) {
            return nullptr;
        }
        if (const auto &record = provider->getRecord(); !record.numbers.empty()) {
            return [app, number = record.numbers[0].number]() {
                app::manager::Controller::sendAction(
                    app, app::manager::actions::Call, std::make_unique<app::ExecuteCallData>(number));
            };
        }
        return nullptr;
    }

    void setCallOnInputCallback(NotificationListItem *item, Notification provider, app::ApplicationCommon *app)
    {
        auto onRightFunctionalKeyCallback = createCallOnRightFunctionalCallback(app);
        auto onLeftFunctionalKeyCallback  = createCallOnLeftFunctionalCallback(app, provider);

        item->inputCallback = [keyRightFunctionalCb = std::move(onRightFunctionalKeyCallback),
                               keyLeftFunctionalCb  = std::move(onLeftFunctionalKeyCallback)](
                                  [[maybe_unused]] Item &item, const InputEvent &inputEvent) {
            if (inputEvent.isShortRelease()) {
                if (inputEvent.is(KeyCode::KEY_RF) && keyRightFunctionalCb != nullptr) {
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
    }

    void setCallDismissCallback(NotificationListItem *item, app::ApplicationCommon *app)
    {
        item->dismissCallback = createCallOnRightFunctionalCallback(app);
    }

    void setTetheringActivatedCallback(NotificationListItem *item, app::ApplicationCommon *app)
    {
        item->activatedCallback = [app]([[maybe_unused]] gui::Item &_item) {
            app->switchWindow(gui::popup::window::tethering_off_window);
            return true;
        };
    }

    void setTetheringFocusChangedCallback(NotificationListItem *item, ActiveNotificationsModel *model)
    {
        item->focusChangedCallback = [model](gui::Item &_item) {
            if (_item.focus) {
                model->setParentBottomBar({}, utils::translate("common_disconnect"), {});
                return true;
            }
            return false;
        };
    }

    void setSnoozeFocusChangedCallback(NotificationListItem *item, ActiveNotificationsModel *model)
    {
        item->focusChangedCallback = [model](gui::Item &_item) {
            if (_item.focus) {
                model->setParentBottomBar({}, utils::translate("app_desktop_show"), utils::translate("common_stop"));
            }
            return true;
        };
    }

    void setSnoozeActivatedCallback(NotificationListItem *item,
                                    ActiveNotificationsModel *model,
                                    app::ApplicationCommon *app)
    {
        item->activatedCallback = [model, app]([[maybe_unused]] gui::Item &_item) {
            auto request = std::make_unique<alarms::GetSnoozedAlarmsRequestMessage>();
            auto task    = app::AsyncRequest::createFromMessage(std::move(request), service::name::service_time);
            auto cb      = [&](auto response) {
                auto result = dynamic_cast<alarms::GetSnoozedAlarmsResponseMessage *>(response);
                assert(result);
                if (!result || result->snoozedAlarms.empty()) {
                    return false;
                }
                app::manager::Controller::sendAction(
                    app,
                    app::manager::actions::ShowPopup,
                    std::make_unique<gui::AlarmPopupRequestParams>(AlarmPopupType::SnoozeCheck,
                                                                   std::move(result->snoozedAlarms)));
                return true;
            };
            task->execute(app, model, cb);
            return true;
        };
    }

    void setSnoozeOnInputCallback(NotificationListItem *item, app::ApplicationCommon *app)
    {
        item->inputCallback = [app]([[maybe_unused]] Item &item, const InputEvent &inputEvent) {
            if (inputEvent.isShortRelease() && inputEvent.is(KeyCode::KEY_RF)) {
                alarms::AlarmServiceAPI::requestStopAllSnoozedAlarms(app);
                return true;
            }
            return false;
        };
    }

    void setSnoozeDismissCallback(NotificationListItem *item, app::ApplicationCommon *app)
    {
        item->dismissCallback = [app]() { alarms::AlarmServiceAPI::requestStopAllSnoozedAlarms(app); };
    }
} // namespace

ActiveNotificationsModel::ActiveNotificationsModel(AppWindow *parent)
    : AsyncCallbackReceiver(parent->getApplication()), parent(parent)
{}

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
    setSMSFocusChangedCallback(item, this);
    setSMSActivatedCallback(item, notification, parent->getApplication());
    setSMSOnInputCallback(item, parent->getApplication());
    setSMSDismissCallback(item, parent->getApplication());
    item->setDismissible(true);
    return item;
}
auto ActiveNotificationsModel::create(const notifications::NotSeenCallNotification *notification)
    -> NotificationListItem *
{
    auto item = NotificationsModel::create(notification);
    setCallFocusChangedCallback(item, notification, this);
    setCallActivatedCallback(item, parent->getApplication());
    setCallOnInputCallback(item, notification, parent->getApplication());
    setCallDismissCallback(item, parent->getApplication());
    item->setDismissible(true);
    return item;
}

auto ActiveNotificationsModel::create(const notifications::TetheringNotification *notification)
    -> NotificationListItem *
{
    auto item               = NotificationsModel::create(notification);
    setTetheringActivatedCallback(item, parent->getApplication());
    setTetheringFocusChangedCallback(item, this);
    return item;
}

auto ActiveNotificationsModel::create(const notifications::AlarmSnoozeNotification *notification)
    -> NotificationListItem *
{
    auto item = NotificationsModel::create(notification);
    setSnoozeFocusChangedCallback(item, this);
    setSnoozeActivatedCallback(item, this, parent->getApplication());
    setSnoozeOnInputCallback(item, parent->getApplication());
    setSnoozeDismissCallback(item, parent->getApplication());
    item->setDismissible(true);
    return item;
}
