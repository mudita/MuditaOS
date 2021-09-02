// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMessages.hpp"
#include "MessagesMainWindow.hpp"
#include "MessagesStyle.hpp"
#include "SearchStart.hpp"
#include "SMSdata.hpp"
#include "ThreadItem.hpp"

#include <application-phonebook/data/PhonebookItemData.hpp>
#include <header/AddElementAction.hpp>
#include <header/SearchAction.hpp>
#include <i18n/i18n.hpp>
#include <log.hpp>
#include <module-db/queries/messages/threads/QueryThreadGetByContactID.hpp>
#include <module-db/queries/notifications/QueryNotificationsClear.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <Style.hpp>

#include <cassert>
#include <memory>

namespace gui
{
    MessagesMainWindow::MessagesMainWindow(app::Application *app)
        : AppWindow(app, gui::name::window::main_window), app::AsyncCallbackReceiver{app}
    {
        buildInterface();
    }

    void MessagesMainWindow::rebuild()
    {
        if (list == nullptr) {
            return;
        }
        list->rebuildList(gui::listview::RebuildType::InPlace);
    }

    void MessagesMainWindow::buildInterface()
    {
        namespace msgThreadStyle = style::messages::threads;

        AppWindow::buildInterface();

        threadsModel = std::make_shared<ThreadsModel>(application);

        list = new gui::ListView(this,
                                 msgThreadStyle::listPositionX,
                                 msgThreadStyle::ListPositionY,
                                 msgThreadStyle::listWidth,
                                 msgThreadStyle::listHeight,
                                 threadsModel,
                                 listview::ScrollBarType::Fixed);
        list->setScrollTopMargin(style::margins::small);
        list->rebuildList();

        bottomBar->setActive(BottomBar::Side::LEFT, true);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::LEFT, utils::translate(style::strings::common::options));
        bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::open));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        setTitle(utils::translate("app_messages_title_main"));
        header->navigationIndicatorAdd(new gui::header::AddElementAction(), gui::header::BoxSelection::Left);
#if MESSAGE_TEXT_SEARCH == 1
        header->navigationIndicatorAdd(new gui::header::SearchAction(), gui::header::BoxSelection::Right);
#endif

        emptyListIcon = new Icon(this,
                                 0,
                                 style::window::default_vertical_pos,
                                 style::window_width,
                                 style::window_height - style::window::default_vertical_pos - style::footer::height,
                                 "phonebook_empty_grey_circle_W_G",
                                 utils::translate("app_messages_no_messages"));

        list->setVisible(true);
        list->focusChangedCallback = [this]([[maybe_unused]] gui::Item &item) {
            bottomBar->setActive(BottomBar::Side::LEFT, true);
            bottomBar->setActive(BottomBar::Side::CENTER, true);
            return true;
        };

        list->emptyListCallback = [this]() {
            emptyListIcon->setVisible(true);
            bottomBar->setActive(BottomBar::Side::LEFT, false);
            bottomBar->setActive(BottomBar::Side::CENTER, false);
        };

        list->notEmptyListCallback = [this]() {
            emptyListIcon->setVisible(false);
            bottomBar->setActive(BottomBar::Side::LEFT, true);
            bottomBar->setActive(BottomBar::Side::CENTER, true);
        };

        setFocusItem(list);
    }

    void MessagesMainWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        LOG_INFO("Data: %s", data ? data->getDescription().c_str() : "");
        {
            auto pdata = dynamic_cast<PhonebookSearchRequest *>(data);
            if (pdata != nullptr) {
                using db::query::ThreadGetByContactID;
                auto query = std::make_unique<ThreadGetByContactID>(pdata->result->ID);
                auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Contact);
                task->setCallback([app = application](auto response) {
                    using db::query::ThreadGetByContactIDResult;
                    const auto result = dynamic_cast<ThreadGetByContactIDResult *>(response);
                    if ((result != nullptr) && result->getRecord().has_value()) {
                        auto thread = result->getRecord().value();
                        app->switchWindow(gui::name::window::thread_view,
                                          std::make_unique<SMSThreadData>(std::make_unique<ThreadRecord>(thread)));
                        return true;
                    }
                    LOG_FATAL("No thread and thread not created!");
                    return false;
                });
                task->execute(application, this);
            }
        }

        DBServiceAPI::GetQuery(application,
                               db::Interface::Name::Notifications,
                               std::make_unique<db::query::notifications::Clear>(NotificationsRecord::Key::Sms));
    }

    bool MessagesMainWindow::onInput(const InputEvent &inputEvent)
    {
        // check if any of the lower inheritance onInput methods catch the event
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }
        if (inputEvent.isShortRelease()) {
            switch (inputEvent.getKeyCode()) {
            case gui::KeyCode::KEY_LEFT:
                application->switchWindow(gui::name::window::new_sms, nullptr);
                return true;
#if MESSAGE_TEXT_SEARCH == 1
            case gui::KeyCode::KEY_RIGHT:
                application->switchWindow(gui::name::window::thread_sms_search, nullptr);
                return true;
#endif
            default:
                LOG_DEBUG("SMS main window not handled key: %s", c_str(inputEvent.getKeyCode()));
                break;
            }
        }
        return false;
    }

    bool MessagesMainWindow::onDatabaseMessage(sys::Message *msgl)
    {
        auto msgNotification = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msgNotification != nullptr) {
            if (msgNotification->interface == db::Interface::Name::SMSThread ||
                msgNotification->interface == db::Interface::Name::SMS) {
                if (msgNotification->dataModified()) {
                    rebuild();
                    return true;
                }
            }
        }
        return false;
    }
} // namespace gui
