// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMessages.hpp"
#include "MessagesStyle.hpp"
#include "SMSdata.hpp"
#include "SMSThreadViewWindow.hpp"
#include "service-appmgr/Controller.hpp"

#include <log/log.hpp>
#include <module-db/queries/phonebook/QueryContactGetByNumberID.hpp>
#include <module-db/queries/phonebook/QueryNumberGetByID.hpp>
#include <service-db/DBNotificationMessage.hpp>
#include <Service/Message.hpp>
#include <Style.hpp>
#include <TextBubble.hpp>

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

        // Text for Old Messages
        oldMessagesText = new gui::Text(this, 0, 0, 0, 0, ExpandMode::None);
        oldMessagesText->setText(utils::translate("app_messages_old_messages"));
        oldMessagesText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        oldMessagesText->setMaximumSize(style::window::default_body_width, style::messages::oldSmsLabel::h);
        oldMessagesText->setFont(style::window::font::verysmall);

        // Arrow sign for Old Messages
        oldMessagesArrow = new gui::Image("old_messages_arrow_24px_W_M");

        // HBox for Old Messages
        oldMessagesHBox = new gui::HBox(this,
                                        style::messages::oldSmsLabel::x,
                                        style::messages::oldSmsLabel::y,
                                        style::messages::oldSmsLabel::w,
                                        style::messages::oldSmsLabel::h);
        oldMessagesHBox->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        oldMessagesHBox->setEdges(gui::RectangleEdge::None);
        oldMessagesHBox->addWidget(oldMessagesText);
        oldMessagesHBox->addWidget(oldMessagesArrow);
        oldMessagesHBox->visible = false;

        // Set callback to switch on/off visible of Old Message Label
        smsList->onElementsAboveOfCurrentPageChangeCallback = [this](const signed int elementsAboveOfCurrentPageCount) {
            oldMessagesHBox->visible = (elementsAboveOfCurrentPageCount > 0);
        };

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
        if (mode == ShowMode::GUI_SHOW_RETURN) {
            smsModel->markCurrentThreadAsRead();
            smsList->rebuildList();
        }

        if (const auto pdata = dynamic_cast<SMSThreadData *>(data); pdata != nullptr) {
            LOG_DEBUG("Thread data received: %" PRIu32, pdata->thread->ID);
            saveInfoAboutPreviousAppForProperSwitchBack(data);

            smsModel->numberID    = pdata->thread->numberID;
            smsModel->smsThreadID = pdata->thread->ID;
            requestContact(smsModel->numberID);

            // Mark thread as read
            if (pdata->thread->isUnread() && (this == application->getCurrentWindow())) {
                smsModel->markCurrentThreadAsRead();
            }

            smsList->rebuildList();
        }
        else if (smsModel->numberID != DB_ID_NONE) {
            requestContact(smsModel->numberID);
        }

        if (const auto pdata = dynamic_cast<SMSTextData *>(data); pdata != nullptr) {
            LOG_DEBUG("Received sms templates data");
            if (pdata->concatenate == SMSTextData::Concatenate::True) {
                smsModel->smsInput->inputText->addText(pdata->text);
            }
            else {
                smsModel->smsInput->inputText->setText(pdata->text);
            }
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
        if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            onClose(CloseReason::WindowSwitch);
            if (shouldCurrentAppBeIgnoredOnSwitchBack()) {
                return app::manager::Controller::switchBack(application,
                                                            std::make_unique<app::manager::SwitchBackRequest>(
                                                                nameOfPreviousApplication.value(), nullptr, true));
            }
        }
        return AppWindow::onInput(inputEvent);
    }

    void SMSThreadViewWindow::onClose([[maybe_unused]] CloseReason reason)
    {
        smsModel->handleDraftMessage();
    }

    bool SMSThreadViewWindow::onDatabaseMessage(sys::Message *msgl)
    {
        const auto msg = dynamic_cast<db::NotificationMessage *>(msgl);
        if (msg == nullptr) {
            return false;
        }

        if ((msg->interface == db::Interface::Name::SMS) && msg->dataModified()) {
            rebuild();
            return true;
        }

        if ((msg->interface == db::Interface::Name::SMSThread) && (this == application->getCurrentWindow())) {
            if (msg->type == db::Query::Type::Delete) {
                application->switchWindow(gui::name::window::main_window);
            }
            else {
                smsModel->markCurrentThreadAsRead();
            }
        }
        return false;
    }

    void SMSThreadViewWindow::requestContact(unsigned int numberID)
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

    bool SMSThreadViewWindow::handleContactQueryResponse(db::QueryResult *queryResult)
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

    bool SMSThreadViewWindow::handleNumberQueryResponse(db::QueryResult *queryResult)
    {
        auto msgResponse = dynamic_cast<db::query::NumberGetByIDResult *>(queryResult);
        if (msgResponse == nullptr) {
            return false;
        }
        setTitle(msgResponse->getNumber().getFormatted().c_str());
        return true;
    }
} /* namespace gui */
