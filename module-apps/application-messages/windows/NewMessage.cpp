// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewMessage.hpp"

#include "application-messages/ApplicationMessages.hpp"
#include "application-messages/data/SMSdata.hpp"
#include "application-messages/data/MessagesStyle.hpp"

#include <application-phonebook/ApplicationPhonebook.hpp>
#include <application-phonebook/windows/PhonebookSearchResults.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <i18n/i18n.hpp>
#include <BoxLayout.hpp>
#include <Text.hpp>

#include <module-db/queries/messages/threads/QueryThreadGetByContactID.hpp>
#include <module-db/queries/messages/threads/QueryThreadGetByNumber.hpp>
#include <module-db/queries/messages/sms/QuerySMSGetLastByThreadID.hpp>

#include <cassert>

namespace gui
{
    class NewMessageWindow::MessageMemento
    {
      public:
        void setState(const gui::Text *_state)
        {
            assert(_state);
            state = std::make_unique<TextBackup>(_state->backupText());
        }

        void getState(gui::Text *_state)
        {
            assert(_state);
            if (!state) {
                return;
            }
            _state->restoreFrom(*state);
            state = nullptr;
        }

      private:
        std::unique_ptr<gui::TextBackup> state;
    };

    std::unique_ptr<NewMessageWindow::MessageMemento> NewMessageWindow::memento =
        std::make_unique<NewMessageWindow::MessageMemento>();

    NewMessageWindow::NewMessageWindow(app::Application *app)
        : AppWindow(app, name::window::new_sms), app::AsyncCallbackReceiver{app}
    {
        buildInterface();
    }

    bool NewMessageWindow::onInput(const InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }

    void NewMessageWindow::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        memento->getState(message);
        if (data == nullptr) {
            return;
        }

        if (auto searchRequest = dynamic_cast<PhonebookSearchRequest *>(data); searchRequest != nullptr) {
            LOG_INFO("Received search results");
            contact = searchRequest->result;
            recipient->setText(contact->getFormattedName());
        }
        else if (auto textData = dynamic_cast<SMSTextData *>(data); textData != nullptr) {
            const auto &text = textData->text;
            LOG_INFO("Received sms text data \"%s\"", text.c_str());
            if (textData->concatenate == SMSTextData::Concatenate::True) {
                message->addText(text);
            }
            else {
                message->setText(text);
            }
            setFocusItem(message);
        }
        else if (auto sendRequest = dynamic_cast<SMSSendRequest *>(data); sendRequest != nullptr) {
            phoneNumber = sendRequest->getPhoneNumber();
            LOG_INFO("Received sms send request to number: %s", phoneNumber.getFormatted().c_str());
            auto retContact = DBServiceAPI::MatchContactByPhoneNumber(application, phoneNumber);
            if (!retContact) {
                LOG_WARN("Not valid contact for number %s", phoneNumber.getEntered().c_str());
                recipient->setText(phoneNumber.getFormatted());
                message->setText(sendRequest->textData);
                return;
            }

            contact = std::move(retContact);
            recipient->setText(contact->getFormattedName());
            message->setText(sendRequest->textData);
        }

        updateBottomBar();
    }

    bool NewMessageWindow::selectContact()
    {
        // select contact only if there is no entered number
        if (recipient->getText().empty()) {
            memento->setState(message);
            return app::manager::Controller::sendAction(application,
                                                        app::manager::actions::ShowContacts,
                                                        std::make_unique<PhonebookSearchRequest>(),
                                                        app::manager::OnSwitchBehaviour::RunInBackground);
        }
        return true;
    }

    bool NewMessageWindow::sendSms()
    {
        auto app = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app != nullptr);

        const auto &number = getPhoneNumber();
        if (const auto success = app->sendSms(number.getView(), message->getText()); !success) {
            LOG_ERROR("Failed to send the SMS.");
            return false;
        }
        if (!Store::GSM::get()->simCardInserted()) {
            auto action = [this, number]() {
                if (!switchToThreadWindow(number.getView())) {
                    LOG_ERROR("switchToThreadWindow failed");
                }
                return true;
            };
            app->showNotification(action, true);
            return true;
        }
        return switchToThreadWindow(number.getView());
    }

    auto NewMessageWindow::getPhoneNumber() const -> utils::PhoneNumber
    {
        if (phoneNumber.getEntered().empty()) {
            if (contact && !(contact->numbers.empty())) {
                return contact->numbers.front().number;
            }
            return utils::PhoneNumber{recipient->getText()};
        }
        return phoneNumber;
    }

    bool NewMessageWindow::switchToThreadWindow(const utils::PhoneNumber::View &number)
    {
        auto thread = DBServiceAPI::ThreadGetByNumber(application, number, getThreadTimeout.count());
        if (!thread) {
            LOG_FATAL("No thread and thread not created!");
            return false;
        }

        // clear data only when message is sent
        contact = nullptr;
        phoneNumber.clear();
        recipient->clear();
        message->clear();
        setFocusItem(body);

        auto switchData                        = std::make_unique<SMSThreadData>(std::move(thread));
        switchData->ignoreCurrentWindowOnStack = true;
        application->switchWindow(gui::name::window::thread_view, std::move(switchData));
        return true;
    }

    void NewMessageWindow::updateBottomBar()
    {
        if (getFocusItem() == recipient) {
            if (recipient->getText().empty()) {
                bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
                return;
            }
            bottomBar->setActive(BottomBar::Side::CENTER, false);
        }
    }

    void NewMessageWindow::buildInterface()
    {
        namespace msgStyle = style::messages::newMessage;
        AppWindow::buildInterface();
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get(style::strings::common::options));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        setTitle(utils::localize.get("sms_title_message"));

        const uint32_t w = this->getWidth() - style::window::default_left_margin - style::window::default_right_margin;
        const uint32_t h = this->getHeight() - title->offset_h() - bottomBar->getHeight();
        body             = new gui::VBox(this, style::window::default_left_margin, (uint32_t)title->offset_h(), w, h);

        auto recipientLabel = new Label(body, 0, 0, body->getWidth(), msgStyle::recipientLabel::h);
        recipientLabel->setText(utils::localize.get("sms_add_rec_num"));
        recipientLabel->activeItem = false;
        recipientLabel->setEdges(gui::RectangleEdge::None);
        recipientLabel->setFont(style::window::font::small);
        recipientLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));

        auto reciepientHbox = new gui::HBox(body, 0, 0, body->getWidth(), msgStyle::text::h);
        reciepientHbox->setAlignment(gui::Alignment::Vertical::Center);
        reciepientHbox->setEdges(gui::RectangleEdge::Bottom);
        reciepientHbox->setPenFocusWidth(style::window::default_border_focus_w);
        reciepientHbox->setPenWidth(style::window::default_border_rect_no_focus);

        recipient = new gui::Text(reciepientHbox,
                                  0,
                                  0,
                                  body->getWidth() - msgStyle::recipientImg::w,
                                  msgStyle::text::h,
                                  "",
                                  ExpandMode::None);
        recipient->setEdges(gui::RectangleEdge::None);
        recipient->setInputMode(new InputMode({InputMode::phone}));
        recipient->setFont(style::window::font::mediumbold);
        recipient->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        recipient->activatedCallback    = [=](Item &) -> bool { return selectContact(); };
        recipient->focusChangedCallback = [=](Item &) -> bool {
            updateBottomBar();
            bottomBar->setActive(BottomBar::Side::LEFT, false);
            return true;
        };
        recipient->setTextChangedCallback([this]([[maybe_unused]] Item &item, const UTF8 &text) {
            if (recipient->getText().empty()) {
                contact = nullptr;
            }
        });

        auto img        = new gui::Image(reciepientHbox, 0, 0, 0, 0, "phonebook_small");
        img->activeItem = false;

        auto labelMessage = new Label(body, 0, 0, body->getWidth(), msgStyle::messageLabel::h);
        labelMessage->setText(utils::localize.get("app_messages_message"));
        labelMessage->activeItem = false;
        labelMessage->setEdges(gui::RectangleEdge::None);
        labelMessage->setFont(style::window::font::small);
        labelMessage->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));

        message = new gui::Text(nullptr, 0, 0, body->getWidth(), msgStyle::text::h, "", ExpandMode::Up);
        message->setMaximumSize(body->getWidth(), body->getHeight());
        message->setEdges(gui::RectangleEdge::Bottom);
        message->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc, InputMode::Abc, InputMode::digit},
            [=](const UTF8 &text) { bottomBarTemporaryMode(text); },
            [=]() { bottomBarRestoreFromTemporaryMode(); },
            [=]() { selectSpecialCharacter(); }));
        message->setPenFocusWidth(style::window::default_border_focus_w);
        message->setPenWidth(style::window::default_border_rect_no_focus);
        message->setFont(style::window::font::medium);
        message->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        message->activatedCallback = [=](Item &) -> bool {
            if (!sendSms()) {
                LOG_ERROR("sendSms failed");
            }
            return true;
        };
        message->focusChangedCallback = [=](Item &) -> bool {
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::send));
            bottomBar->setActive(BottomBar::Side::LEFT, true);
            return true;
        };
        message->inputCallback = [=](Item &, const InputEvent &event) {
            if (event.state == InputEvent::State::keyReleasedShort && event.keyCode == KeyCode::KEY_LF) {
                auto app = dynamic_cast<app::ApplicationMessages *>(application);
                assert(app != nullptr);
                memento->setState(message);
                return app->newMessageOptions(getName(), message);
            }
            return false;
        };
        body->addWidget(message);
        body->addWidget(new Span(Axis::Y, body->getHeight()));
        message->setText(""); // to set actual size of Text

        body->setEdges(gui::RectangleEdge::None);
        body->setVisible(true);
        body->setNavigation();
        setFocusItem(body);
    }

    void NewMessageWindow::onClose()
    {
        if (message->getText().empty()) {
            // Nothing to do if text is empty.
            return;
        }
        if (const auto handled = handleMessageText(); !handled) {
            message->clear();
        }
    }

    auto NewMessageWindow::handleMessageText() -> bool
    {
        if (contact && !(contact->numbers.empty())) {
            return addDraft(*contact);
        }
        if (const auto &number = getPhoneNumber(); !number.get().empty()) {
            return addDraft(number);
        }
        return false;
    }

    auto NewMessageWindow::addDraft(const ContactRecord &contactRecord) -> bool
    {
        auto number = contactRecord.numbers.front().number;
        auto query  = std::make_unique<db::query::ThreadGetByContactID>(contactRecord.ID);
        auto task   = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::SMSThread);
        task->setCallback([this, number](auto response) {
            const auto result = dynamic_cast<db::query::ThreadGetByContactIDResult *>(response);
            if (result == nullptr) {
                return false;
            }

            const auto &thread = result->getRecord();
            if (!thread.has_value()) {
                storeMessageDraft(number, message->getText());
                return true;
            }
            return addDraftToExistingThread(thread->ID, number, message->getText());
        });
        task->execute(application, this);
        return true;
    }

    auto NewMessageWindow::addDraft(const utils::PhoneNumber &number) -> bool
    {
        auto query = std::make_unique<db::query::ThreadGetByNumber>(number.getView());
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::SMSThread);
        task->setCallback([this, number](auto response) {
            const auto result = dynamic_cast<db::query::ThreadGetByNumberResult *>(response);
            if (result == nullptr) {
                return false;
            }

            const auto &thread = result->getThread();
            if (!thread.isValid()) {
                storeMessageDraft(number.getView(), message->getText());
                return true;
            }
            return addDraftToExistingThread(thread.ID, number.getView(), message->getText());
        });
        task->execute(application, this);
        return true;
    }

    auto NewMessageWindow::addDraftToExistingThread(unsigned int threadId,
                                                    const utils::PhoneNumber::View &number,
                                                    const UTF8 &text) -> bool
    {
        auto query = std::make_unique<db::query::SMSGetLastByThreadID>(threadId);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::SMS);
        task->setCallback([this, number](auto response) {
            const auto result = dynamic_cast<db::query::SMSGetLastByThreadIDResult *>(response);
            if (result == nullptr) {
                return false;
            }

            auto lastSms = result->record;
            if (lastSms.has_value() && lastSms->type == SMSType::DRAFT) {
                storeMessageDraft(lastSms.value(), message->getText());
                return true;
            }
            storeMessageDraft(number, message->getText());
            return true;
        });
        task->execute(application, this);
        return true;
    }

    void NewMessageWindow::storeMessageDraft(const utils::PhoneNumber::View &number, const UTF8 &text)
    {
        auto app = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app != nullptr);

        app->createDraft(number, text);
        message->clear();
    }

    void NewMessageWindow::storeMessageDraft(SMSRecord &sms, const UTF8 &text)
    {
        auto app = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app != nullptr);

        app->updateDraft(sms, text);
        message->clear();
    }

} // namespace gui
