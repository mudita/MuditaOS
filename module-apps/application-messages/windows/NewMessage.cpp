﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMessages.hpp"
#include "MessagesStyle.hpp"
#include "NewMessage.hpp"
#include "SMSdata.hpp"

#include <application-phonebook/windows/PhonebookSearchResults.hpp>
#include <Span.hpp>
#include <BoxLayout.hpp>
#include <i18n/i18n.hpp>
#include <module-db/queries/messages/sms/QuerySMSGetLastByThreadID.hpp>
#include <module-db/queries/messages/threads/QueryThreadGetByNumber.hpp>
#include <service-appmgr/Controller.hpp>
#include <service-cellular/service-cellular/MessageConstants.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <Text.hpp>

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

        void restoreState(gui::Text *_state)
        {
            assert(_state);
            if (state == nullptr) {
                return;
            }

            const auto currentText = _state->getText();
            _state->restoreFrom(*state);
            if (!currentText.empty()) {
                _state->addText(currentText);
            }

            state.reset();
        }

      private:
        std::unique_ptr<gui::TextBackup> state;
    };

    std::unique_ptr<NewMessageWindow::MessageMemento> NewMessageWindow::mementoMessage =
        std::make_unique<NewMessageWindow::MessageMemento>();
    std::unique_ptr<NewMessageWindow::MessageMemento> NewMessageWindow::mementoRecipient =
        std::make_unique<NewMessageWindow::MessageMemento>();

    NewMessageWindow::NewMessageWindow(app::ApplicationCommon *app)
        : AppWindow(app, name::window::new_sms), app::AsyncCallbackReceiver{app}
    {
        buildInterface();
    }

    bool NewMessageWindow::onInput(const InputEvent &inputEvent)
    {
        if (!inputEvent.isShortRelease(KeyCode::KEY_RF) || !shouldCurrentAppBeIgnoredOnSwitchBack()) {
            return AppWindow::onInput(inputEvent);
        }

        return app::manager::Controller::switchBack(
            application,
            std::make_unique<app::manager::SwitchBackRequest>(nameOfPreviousApplication.value(), nullptr, true));
    }

    void NewMessageWindow::onBeforeShow([[maybe_unused]] ShowMode mode, SwitchData *data)
    {
        mementoMessage->restoreState(message);
        mementoRecipient->restoreState(recipient);
        if (data == nullptr) {
            return;
        }

        if (const auto searchRequest = dynamic_cast<PhonebookSearchRequest *>(data); searchRequest != nullptr) {
            LOG_INFO("Received search results");
            saveInfoAboutPreviousAppForProperSwitchBack(data);
            contact        = searchRequest->result;
            selectedNumber = searchRequest->selectedNumber;
            recipient->setText(contact->getFormattedName());
        }
        else if (const auto textData = dynamic_cast<SMSTextData *>(data); textData != nullptr) {
            const auto &text = textData->text;
            LOG_INFO("Received sms text data");
            if (textData->concatenate == SMSTextData::Concatenate::True) {
                message->addText(text);
            }
            else {
                message->setText(text);
            }
            setFocusItem(message);
        }
        else if (const auto sendRequest = dynamic_cast<SMSSendRequest *>(data); sendRequest != nullptr) {
            LOG_INFO("Received sms send request");
            saveInfoAboutPreviousAppForProperSwitchBack(data);
            phoneNumber = sendRequest->getPhoneNumber();
            contact     = DBServiceAPI::MatchContactByPhoneNumber(application, phoneNumber);
            if (!contact || contact->isTemporary()) {
                recipient->setText(phoneNumber.getFormatted());
            }
            else {
                recipient->setText(contact->getFormattedName());
            }
            message->setText(sendRequest->textData);
        }

        updateNavBar();
    }

    bool NewMessageWindow::selectContact()
    {
        // select contact only if there is no entered number
        if (recipient->getText().empty()) {
            return app::manager::Controller::sendAction(application,
                                                        app::manager::actions::ShowContacts,
                                                        std::make_unique<PhonebookSearchRequest>(),
                                                        app::manager::OnSwitchBehaviour::RunInBackground);
        }
        return true;
    }

    bool NewMessageWindow::sendSms()
    {
        const auto app = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app != nullptr);

        const auto &number = getPhoneNumber();
        if (const auto success = app->sendSms(number.getView(), message->getText()); !success) {
            LOG_ERROR("Failed to send the SMS.");
            return false;
        }
        setFocusItem(nullptr); // to make sure it is not possible to send text message multiple times
        return switchToThreadWindow(number.getView());
    }

    auto NewMessageWindow::getPhoneNumber() const -> utils::PhoneNumber
    {
        if (phoneNumber.getEntered().empty()) {
            if (contact && !contact->numbers.empty()) {
                return contact->numbers.at(selectedNumber).number;
            }
            return utils::PhoneNumber{recipient->getText()};
        }
        return phoneNumber;
    }

    bool NewMessageWindow::switchToThreadWindow(const utils::PhoneNumber::View &number)
    {
        auto query = std::make_unique<db::query::ThreadGetByNumber>(number);
        const auto task = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::SMSThread);
        task->setCallback([this](auto response) {
            const auto result = dynamic_cast<db::query::ThreadGetByNumberResult *>(response);
            if (result == nullptr) {
                return false;
            }
            // clear data only when message is sent
            contact = nullptr;
            phoneNumber.clear();
            recipient->clear();
            message->clear();
            setFocusItem(body);

            const auto &thread = result->getThread();
            auto switchData    = std::make_unique<SMSThreadData>(std::make_unique<ThreadRecord>(thread));
            switchData->ignoreCurrentWindowOnStack = true;
            switchData->nameOfSenderApplication    = nameOfPreviousApplication;
            application->switchWindow(gui::name::window::thread_view, std::move(switchData));
            return true;
        });
        task->execute(application, this);

        return true;
    }

    void NewMessageWindow::updateNavBar()
    {
        if (getFocusItem() == recipient) {
            navBar->setActive(nav_bar::Side::Left, false);
            if (recipient->getText().empty()) {
                navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::contacts));
                return;
            }
            navBar->setActive(nav_bar::Side::Center, false);
        }
    }

    void NewMessageWindow::buildInterface()
    {
        namespace msgStyle = style::messages::newMessage;
        AppWindow::buildInterface();
        navBar->setText(nav_bar::Side::Left, utils::translate(style::strings::common::options));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        setTitle(utils::translate("sms_title_message"));

        const uint32_t w = this->getWidth() - style::window::default_left_margin - style::window::default_right_margin;
        const uint32_t h = this->getHeight() - style::window::default_vertical_pos - navBar->getHeight();
        body = new gui::VBox(this, style::window::default_left_margin, style::window::default_vertical_pos, w, h);

        const auto recipientLabel = new Label(body, 0, 0, body->getWidth(), msgStyle::recipientLabel::h);
        recipientLabel->setText(utils::translate("sms_add_rec_num"));
        recipientLabel->activeItem = false;
        recipientLabel->setEdges(gui::RectangleEdge::None);
        recipientLabel->setFont(style::window::font::small);
        recipientLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));

        const auto recipientHBox = new gui::HBox(body, 0, 0, body->getWidth(), msgStyle::text::h);
        recipientHBox->setAlignment(gui::Alignment::Vertical::Center);
        recipientHBox->setEdges(gui::RectangleEdge::Bottom);
        recipientHBox->setPenFocusWidth(style::window::default_border_focus_w);
        recipientHBox->setPenWidth(style::window::default_border_rect_no_focus);

        recipient = new gui::Text(
            recipientHBox, 0, 0, body->getWidth() - msgStyle::recipientImg::w, msgStyle::text::h, ExpandMode::None);
        recipient->setEdges(gui::RectangleEdge::None);
        recipient->setInputMode(new InputMode({InputMode::phone}));
        recipient->setFont(style::window::font::mediumbold);
        recipient->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        recipient->activatedCallback    = [=](Item &) -> bool { return selectContact(); };
        recipient->focusChangedCallback = [=](Item &) -> bool {
            updateNavBar();
            return true;
        };
        recipient->inputCallback = [this]([[maybe_unused]] Item &, const InputEvent &inputEvent) -> bool {
            if (contact != nullptr) {
                if (inputEvent.isShortRelease(KeyCode::KEY_PND)) {
                    recipient->clear();
                    return true;
                }
                if (inputEvent.isDigit()) {
                    return true;
                }
            }
            if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
                onClose(CloseReason::WindowSwitch);
            }
            return false;
        };
        recipient->setTextChangedCallback([this]([[maybe_unused]] Item &item, [[maybe_unused]] const UTF8 &text) {
            if (recipient->getText().empty()) {
                contact = nullptr;
                phoneNumber.clear();
            }
            updateNavBar();
        });

        const auto img  = new gui::Image(recipientHBox, 0, 0, 0, 0, "phonebook_32px_W_G");
        img->activeItem = false;

        const auto labelMessage = new Label(body, 0, 0, body->getWidth(), msgStyle::messageLabel::h);
        labelMessage->setText(utils::translate("app_messages_message"));
        labelMessage->activeItem = false;
        labelMessage->setEdges(gui::RectangleEdge::None);
        labelMessage->setFont(style::window::font::small);
        labelMessage->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Bottom));

        message = new gui::Text(nullptr, 0, 0, body->getWidth(), msgStyle::text::h, ExpandMode::Up);
        message->setMaximumSize(body->getWidth(), msgStyle::text::maxH);
        message->setTextLimitType(gui::TextLimitType::MaxSignsCount, msgConstants::maxConcatenatedLen);
        message->setEdges(gui::RectangleEdge::Bottom);
        message->setInputMode(new InputMode(
            {InputMode::Abc, InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &text) { navBarTemporaryMode(text); },
            [=]() { navBarRestoreFromTemporaryMode(); },
            [=]() { selectSpecialCharacter(); },
            [=](std::function<void()> restoreFunction) {
                application->getCurrentWindow()->startInputModeRestoreTimer(std::move(restoreFunction));
            }));
        message->setPenFocusWidth(style::window::default_border_focus_w);
        message->setPenWidth(style::window::default_border_rect_no_focus);
        message->setFont(style::window::font::medium);
        message->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Center));
        message->activatedCallback = [=](Item &) -> bool {
            if (recipient->getText().empty() || message->getText().empty()) {
                return false;
            }
            if (!sendSms()) {
                LOG_ERROR("Send SMS failed");
            }
            return true;
        };
        message->focusChangedCallback = [=](Item &) -> bool {
            if (recipient->getText().empty()) {
                navBar->setActive(nav_bar::Side::Center, false);
            }
            else {
                navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::send));
            }
            navBar->setActive(nav_bar::Side::Left, true);
            return true;
        };
        message->inputCallback = [=](Item &, const InputEvent &event) {
            if (event.isShortRelease(KeyCode::KEY_LF)) {
                const auto app = dynamic_cast<app::ApplicationMessages *>(application);
                assert(app != nullptr);
                mementoMessage->setState(message);
                mementoRecipient->setState(recipient);
                return app->newMessageOptions(getName(), message);
            }
            if (event.isShortRelease(KeyCode::KEY_RF)) {
                onClose(CloseReason::WindowSwitch);
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

    void NewMessageWindow::onClose(CloseReason reason)
    {
        if (message->getText().empty()) {
            // Nothing to do if text is empty.
            return;
        }

        if (reason != CloseReason::ApplicationClose) {
            mementoMessage->setState(message);
            mementoRecipient->setState(recipient);
            message->clear();
            recipient->clear();
            return;
        }

        if (const auto handled = handleMessageText(); !handled) {
            message->clear();
        }
    }

    auto NewMessageWindow::handleMessageText() -> bool
    {
        if (const auto &number = getPhoneNumber(); !number.get().empty()) {
            return addDraft(number);
        }
        return false;
    }

    auto NewMessageWindow::addDraft(const utils::PhoneNumber &number) -> bool
    {
        auto query = std::make_unique<db::query::ThreadGetByNumber>(number.getView());
        const auto task = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::SMSThread);
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
        task->execute(application, this, std::nullopt, app::ReceiverBehavior::WaitForResponseToClose);
        return true;
    }

    auto NewMessageWindow::addDraftToExistingThread(unsigned int threadId,
                                                    const utils::PhoneNumber::View &number,
                                                    const UTF8 &text) -> bool
    {
        auto query = std::make_unique<db::query::SMSGetLastByThreadID>(threadId);
        const auto task = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::SMS);
        task->setCallback([this, number](const auto response) {
            const auto result = dynamic_cast<db::query::SMSGetLastByThreadIDResult *>(response);
            if (result == nullptr) {
                return false;
            }

            auto &lastSms = result->record;
            if (lastSms.has_value() && lastSms->type == SMSType::DRAFT) {
                storeMessageDraft(lastSms.value(), message->getText());
                return true;
            }
            storeMessageDraft(number, message->getText());
            return true;
        });
        task->execute(application, this, std::nullopt, app::ReceiverBehavior::WaitForResponseToClose);
        return true;
    }

    void NewMessageWindow::storeMessageDraft(const utils::PhoneNumber::View &number, const UTF8 &text)
    {
        const auto app = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app != nullptr);

        app->createDraft(number, text);
        message->clear();
    }

    void NewMessageWindow::storeMessageDraft(SMSRecord &sms, const UTF8 &text)
    {
        const auto app = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app != nullptr);

        app->updateDraft(sms, text);
        message->clear();
    }
} // namespace gui
