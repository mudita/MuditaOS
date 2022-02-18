// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMessages.hpp"
#include "MessagesStyle.hpp"
#include "SMSdata.hpp"
#include "SMSThreadViewWindow.hpp"

#include <log/log.hpp>
#include <module-db/queries/notifications/QueryNotificationsDecrement.hpp>
#include <module-db/queries/phonebook/QueryContactGetByNumberID.hpp>
#include <module-db/queries/phonebook/QueryNumberGetByID.hpp>
#include <OptionsMessages.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <Service/Message.hpp>
#include <Style.hpp>
#include <TextBubble.hpp>

#include <cassert>

namespace gui
{

    SMSThreadViewWindow::SMSThreadViewWindow(app::ApplicationCommon *app)
        : AppWindow(app, name::window::thread_view), app::AsyncCallbackReceiver{app},
          smsModel{std::make_shared<SMSThreadModel>(app)}
    {
        AppWindow::buildInterface();
        setTitle(utils::translate("app_messages_title_main"));
        navBar->setText(nav_bar::Side::Left, utils::translate(style::strings::common::options));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        smsList = new gui::ListView(this,
                                    style::messages::smsList::x,
                                    style::messages::smsList::y,
                                    style::messages::smsList::w,
                                    style::messages::smsList::h,
                                    smsModel,
                                    listview::ScrollBarType::Proportional);
        smsList->setOrientation(listview::Orientation::BottomTop);

        setFocusItem(smsList);
    }

    void SMSThreadViewWindow::rebuild()
    {
        smsList->rebuildList();
    }

    void SMSThreadViewWindow::buildInterface()
    {}

    void SMSThreadViewWindow::destroyInterface()
    {
        erase();
    }

    SMSThreadViewWindow::~SMSThreadViewWindow()
    {
        destroyInterface();
    }

    void SMSThreadViewWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto pdata = dynamic_cast<SMSThreadData *>(data); pdata) {
            LOG_INFO("Thread data received: %" PRIu32, pdata->thread->ID);
            requestContact(pdata->thread->numberID);

            // Mark thread as Read
            if (pdata->thread->isUnread()) {
                auto app = dynamic_cast<app::ApplicationMessages *>(application);
                assert(app != nullptr);
                if (application->getCurrentWindow() == this) {
                    app->markSmsThreadAsRead(pdata->thread->ID);
                }
            }
            smsModel->numberID    = pdata->thread->numberID;
            smsModel->smsThreadID = pdata->thread->ID;
            smsList->rebuildList();

            if (pdata->thread->unreadMsgCount) {
                const auto countToClear = pdata->thread->unreadMsgCount;

                DBServiceAPI::GetQuery(
                    application,
                    db::Interface::Name::Notifications,
                    std::make_unique<db::query::notifications::Decrement>(NotificationsRecord::Key::Sms, countToClear));
            }
        }
        else if (smsModel->numberID != DB_ID_NONE) {
            requestContact(smsModel->numberID);
        }

        if (auto pdata = dynamic_cast<SMSTextData *>(data); pdata) {
            auto txt = pdata->text;
            LOG_INFO("received sms templates data");
            pdata->concatenate == SMSTextData::Concatenate::True ? smsModel->smsInput->inputText->addText(txt)
                                                                 : smsModel->smsInput->inputText->setText(txt);
        }
    }

    bool SMSThreadViewWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_ENTER) &&
            navBar->getText(nav_bar::Side::Center) !=
                utils::translate(utils::translate(style::strings::common::send))) {

            smsList->rebuildList();
            return true;
        }
        return AppWindow::onInput(inputEvent);
    }

    void SMSThreadViewWindow::onClose([[maybe_unused]] CloseReason reason)
    {
        smsModel->handleDraftMessage();
    }

    bool SMSThreadViewWindow::onDatabaseMessage(sys::Message *msgl)
    {
        auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msg != nullptr) {
            if (msg->interface == db::Interface::Name::SMS) {
                if (msg->dataModified()) {
                    rebuild();
                    return true;
                }
            }
            if (msg->interface == db::Interface::Name::SMSThread) {
                if (msg->type == db::Query::Type::Delete) {
                    if (this == application->getCurrentWindow()) {
                        application->switchWindow(gui::name::window::main_window);
                    }
                }
            }
        }
        return false;
    }

    auto SMSThreadViewWindow::requestContact(unsigned int numberID) -> void
    {
        auto query = std::make_unique<db::query::ContactGetByNumberID>(numberID);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Contact);
        task->setCallback([this, numberID](auto response) {
            if (const auto ret = handleContactQueryResponse(response); !ret) {
                requestNumber(numberID);
                return false;
            }
            return true;
        });
        task->execute(application, this);
    }

    auto SMSThreadViewWindow::handleContactQueryResponse(db::QueryResult *queryResult) -> bool
    {
        auto msgResponse = dynamic_cast<db::query::ContactGetByNumberIDResult *>(queryResult);
        if (msgResponse == nullptr) {
            return false;
        }
        if (auto contactRecord = msgResponse->getResult(); !contactRecord.isValid() || contactRecord.isTemporary()) {
            return false;
        }
        setTitle(msgResponse->getResult().getFormattedName());
        return true;
    }

    void SMSThreadViewWindow::requestNumber(unsigned int numberID)
    {
        auto query = std::make_unique<db::query::NumberGetByID>(numberID);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Contact);
        task->setCallback([this](auto response) { return handleNumberQueryResponse(response); });
        task->execute(application, this);
    }

    auto SMSThreadViewWindow::handleNumberQueryResponse(db::QueryResult *queryResult) -> bool
    {
        auto msgResponse = dynamic_cast<db::query::NumberGetByIDResult *>(queryResult);
        if (msgResponse == nullptr) {
            return false;
        }
        setTitle(msgResponse->getNumber().getFormatted().c_str());
        return true;
    }
} /* namespace gui */
