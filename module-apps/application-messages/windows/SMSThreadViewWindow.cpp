// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SMSThreadViewWindow.hpp"

#include "application-messages/ApplicationMessages.hpp"
#include "application-messages/data/SMSdata.hpp"
#include "application-messages/data/MessagesStyle.hpp"

#include "OptionsMessages.hpp"
#include "Service/Message.hpp"

#include <TextBubble.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <module-db/queries/phonebook/QueryContactGetByID.hpp>

#include <log/log.hpp>
#include <Style.hpp>

#include <cassert>

namespace gui
{

    SMSThreadViewWindow::SMSThreadViewWindow(app::Application *app)
        : AppWindow(app, name::window::thread_view), app::AsyncCallbackReceiver{app},
          smsModel{std::make_shared<SMSThreadModel>(app)}
    {
        AppWindow::buildInterface();
        setTitle(utils::localize.get("app_messages_title_main"));
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get(style::strings::common::options));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        smsList = new gui::ListView(this,
                                    style::messages::smsList::x,
                                    style::messages::smsList::y,
                                    style::messages::smsList::w,
                                    style::messages::smsList::h,
                                    smsModel);
        smsList->setOrientation(style::listview::Orientation::BottomTop);

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
        {
            auto pdata = dynamic_cast<SMSThreadData *>(data);
            if (pdata) {
                LOG_INFO("Thread data received: %" PRIu32, pdata->thread->ID);
                requestContactName(pdata->thread->contactID);

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
            }
        }
        if (auto pdata = dynamic_cast<SMSTextData *>(data)) {
            auto txt = pdata->text;
            LOG_INFO("received sms templates data \"%s\"", txt.c_str());
            pdata->concatenate == SMSTextData::Concatenate::True ? smsModel->smsInput->inputText->addText(txt)
                                                                 : smsModel->smsInput->inputText->setText(txt);
        }
    }

    bool SMSThreadViewWindow::onInput(const InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }

    void SMSThreadViewWindow::onClose()
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

    auto SMSThreadViewWindow::requestContactName(unsigned int contactID) -> void
    {
        auto query = std::make_unique<db::query::ContactGetByID>(contactID, true);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Contact);
        task->setCallback([this](auto response) { return handleContactNameQueryResponse(response); });
        task->execute(application, this);
    }

    auto SMSThreadViewWindow::handleContactNameQueryResponse(db::QueryResult *queryResult) -> bool
    {
        auto msgResponse = dynamic_cast<db::query::ContactGetByIDResult *>(queryResult);
        assert(msgResponse != nullptr);

        setTitle(msgResponse->getResult().getFormattedName());

        return false;
    }
} /* namespace gui */
