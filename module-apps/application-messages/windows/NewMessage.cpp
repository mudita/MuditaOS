#include "NewMessage.hpp"

#include "ThreadViewWindow.hpp"
#include "application-messages/ApplicationMessages.hpp"
#include "application-messages/data/SMSdata.hpp"
#include "application-messages/MessagesStyle.hpp"

#include <application-phonebook/ApplicationPhonebook.hpp>
#include <application-phonebook/windows/PhonebookSearchResults.hpp>
#include <service-appmgr/ApplicationManager.hpp>
#include <service-db/api/DBServiceAPI.hpp>
#include <i18/i18.hpp>
#include <BoxLayout.hpp>
#include <Text.hpp>

#include <cassert>

namespace gui
{
    NewSMS_Window::NewSMS_Window(app::Application *app) : AppWindow(app, name::window::new_sms)
    {
        buildInterface();
    }

    bool NewSMS_Window::onInput(const InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }

    void NewSMS_Window::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (auto pdata = dynamic_cast<PhonebookSearchReuqest *>(data); pdata != nullptr) {
            LOG_INFO("received search results");
            recipient->setText(pdata->result->getFormattedName());
            contact = pdata->result;
        }
        if (auto pdata = dynamic_cast<SMSTextData *>(data); pdata != nullptr) {
            auto text = pdata->text;
            LOG_INFO("received sms templates data \"%s\"", text.c_str());
            message->setText(message->getText() + text);
        }
        if (auto pdata = dynamic_cast<SMSSendRequest *>(data); pdata != nullptr) {
            LOG_INFO("recieved sms send request");
            auto number  = pdata->getPhoneNumber().getEntered();
            auto records = DBServiceAPI::ContactGetByPhoneNumber(application, number);
            if (records->empty()) {
                LOG_WARN("not valid contact for number %s", number.c_str());
                recipient->setText(number);
                return;
            }
            contact = std::make_shared<ContactRecord>((*records)[0]);
            recipient->setText(contact->getFormattedName());
        }
    }

    bool NewSMS_Window::selectContact()
    {
        // select contact only if there is no entered number
        if (recipient->getText().length() == 0) {
            std::unique_ptr<PhonebookSearchReuqest> data = std::make_unique<PhonebookSearchReuqest>();
            data->disableAppClose                        = true;
            return sapm::ApplicationManager::messageSwitchApplication(
                application, app::name_phonebook, name::window::main_window, std::move(data));
        }

        return true;
    }

    bool NewSMS_Window::sendSms()
    {
        auto app = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app != nullptr);
        // if a valid contact was found, choose it. Otherwise, get a raw entered number
        UTF8 number = (contact && contact->numbers.size() != 0) ? contact->numbers[0].numberE164 : recipient->getText();
        auto ret    = app->sendSms(number, message->getText());
        if (!ret) {
            LOG_ERROR("sendSms failed");
            return false;
        }

        uint32_t contactId;
        if (!contact || contact->numbers.size() == 0) {
            // once the sms is send, there is assumption that contact exists
            auto records = DBServiceAPI::ContactGetByPhoneNumber(application, number);
            if (records->empty()) {
                LOG_ERROR("not valid contact for number %s", number.c_str());
                return false;
            }
            contact = std::make_shared<ContactRecord>(records->operator[](0));
        }
        contactId = contact->ID;

        auto thread = DBServiceAPI::ThreadGetByContact(application, contactId);
        if (thread) {
            // clear data only when message is sent
            contact = nullptr;
            recipient->setText("");
            message->setText("");
            setFocusItem(body);
            auto switchData                        = std::make_unique<SMSThreadData>(std::move(thread));
            switchData->ignoreCurrentWindowOnStack = true;
            application->switchWindow(gui::name::window::thread_view, std::move(switchData));
        }
        else {
            LOG_FATAL("No thread and thread not created!");
            return false;
        }

        return true;
    }

    void NewSMS_Window::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get(style::strings::common::options));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        topBar->setActive(TopBar::Elements::BATTERY, false);
        topBar->setActive(TopBar::Elements::SIM, false);
        topBar->setActive(TopBar::Elements::SIGNAL, false);

        setTitle(utils::localize.get("sms_title_message"));

        const uint32_t w = this->getWidth() - style::window::default_left_margin - style::window::default_right_margin;
        const uint32_t h = this->getHeight() - title->offset_h() - bottomBar->getHeight();
        body             = new gui::VBox(this, style::window::default_left_margin, (uint32_t)title->offset_h(), w, h);

        auto recipientLabel = new Label(body, 0, 0, body->getWidth(), ::messages::newMessage::recipientLabel::h);
        recipientLabel->setText(utils::localize.get("sms_add_rec_num"));
        recipientLabel->activeItem = false;
        recipientLabel->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        recipientLabel->setFont(style::window::font::small);
        recipientLabel->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM));

        auto reciepientHbox = new gui::HBox(body, 0, 0, body->getWidth(), ::messages::newMessage::text::h);
        reciepientHbox->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        reciepientHbox->setPenFocusWidth(style::window::default_border_focucs_w);
        reciepientHbox->setPenWidth(style::window::messages::sms_border_no_focus);

        recipient = new gui::Text(reciepientHbox,
                                  0,
                                  0,
                                  body->getWidth() - ::messages::newMessage::recipientImg::w,
                                  ::messages::newMessage::text::h,
                                  "",
                                  gui::Text::ExpandMode::EXPAND_NONE);
        recipient->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        recipient->setInputMode(new InputMode({InputMode::phone}));
        recipient->setFont(style::window::font::mediumbold);
        recipient->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM));
        recipient->activatedCallback    = [=](Item &) -> bool { return selectContact(); };
        recipient->focusChangedCallback = [=](Item &) -> bool {
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
            bottomBar->setActive(BottomBar::Side::LEFT, false);
            return true;
        };

        auto img        = new gui::Image(reciepientHbox, 0, 0, 0, 0, "phonebook_small");
        img->activeItem = false;

        auto labelMessage = new Label(body, 0, 0, body->getWidth(), ::messages::newMessage::messageLabel::h);
        labelMessage->setText(utils::localize.get("app_messages_message"));
        labelMessage->activeItem = false;
        labelMessage->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        labelMessage->setFont(style::window::font::small);
        labelMessage->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM));

        message = new gui::Text(
            nullptr, 0, 0, body->getWidth(), ::messages::newMessage::text::h, "", gui::Text::ExpandMode::EXPAND_UP);
        message->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        message->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &) {},
            [=]() { textSelectSpecialCB(); }));
        message->setPenFocusWidth(style::window::default_border_focucs_w);
        message->setPenWidth(style::window::messages::sms_border_no_focus);
        message->setFont(style::window::font::medium);
        message->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM));
        message->activatedCallback    = [=](Item &) -> bool { return sendSms(); };
        message->focusChangedCallback = [=](Item &) -> bool {
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::send));
            bottomBar->setActive(BottomBar::Side::LEFT, true);
            return true;
        };
        message->inputCallback = [=](Item &, const InputEvent &event) {
            if (event.state == InputEvent::State::keyReleasedShort && event.keyCode == KeyCode::KEY_LF) {
                auto app = dynamic_cast<app::ApplicationMessages *>(application);
                assert(app != nullptr);
                return app->newMessageOptions(getName(), message);
            }
            return false;
        };
        body->addWidget(message);

        body->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        body->setVisible(true);
        body->setNavigation();
        setFocusItem(body);
    }

}; // namespace gui
