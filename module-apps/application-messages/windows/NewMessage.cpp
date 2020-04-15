#include "NewMessage.hpp"

#include "ThreadViewWindow.hpp"
#include "../ApplicationMessages.hpp"
#include "../data/SMSdata.hpp"

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
        if (auto pdata = dynamic_cast<PhonebookSearchReuqest *>(data)) {
            LOG_INFO("received search results");
            recipient->setText(pdata->result->getFormattedName());
            contact = pdata->result;
        }
    }

    void NewSMS_Window::rebuild()
    {}

    void NewSMS_Window::buildInterface()
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::LEFT, true);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("common_options"));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_select"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));
        setTitle(utils::localize.get("sms_title_message"));

        uint32_t w = this->getWidth() - style::window::default_left_margin * 2;
        uint32_t h = this->getHeight() - title->offset_h() - bottomBar->getHeight();
        auto body  = new gui::VBox(this, style::window::default_left_margin, (uint32_t)title->offset_h(), w, h);

        auto label = new Label(body, 0, 0, body->getWidth(), 42);
        label->setText(utils::localize.get("sms_add_rec_num"));
        label->activeItem = false;
        label->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        label->setFont(style::window::font::small);
        label->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM));

        recipient = new gui::Text(nullptr, 0, 0, body->getWidth(), 43, "", gui::Text::ExpandMode::EXPAND_NONE);
        recipient->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        recipient->setInputMode(new InputMode({InputMode::phone}));
        recipient->setPenFocusWidth(style::window::default_border_focucs_w);
        recipient->setPenWidth(style::window::messages::sms_border_no_focus);
        recipient->setFont(style::window::font::medium);
        recipient->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM));
        recipient->activatedCallback = [=](Item &) -> bool {
            std::shared_ptr<std::vector<ContactRecord>> searchResults =
                DBServiceAPI::ContactSearch(application, {}, {}, recipient->getText());
            LOG_INFO("Get contact from another app, contacts matching num: %d",
                     static_cast<int>(searchResults.get()->size()));
            if (searchResults.get()->size() > 0) {
                std::unique_ptr<PhonebookSearchReuqest> data =
                    std::make_unique<PhonebookSearchReuqest>(recipient->getText(), searchResults);
                data->disableAppClose = true;
                return sapm::ApplicationManager::messageSwitchApplication(
                    application, app::name_phonebook, name::window::search_results, std::move(data));
            }
            return true;
        };
        recipient->focusChangedCallback = [=](Item &) -> bool {
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_select"));
            return true;
        };
        body->addWidget(recipient);

        auto labelMessage = new Label(body, 0, 0, body->getWidth(), 44);
        labelMessage->setText(utils::localize.get("app_messages_message"));
        labelMessage->activeItem = false;
        labelMessage->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        labelMessage->setFont(style::window::font::small);
        labelMessage->setAlignement(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM));

        message = new gui::Text(nullptr, 0, 0, body->getWidth(), 43, "", gui::Text::ExpandMode::EXPAND_UP);
        message->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        message->setInputMode(new InputMode(
            {InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &) {},
            [=]() { textSelectSpecialCB(); }));
        message->setPenFocusWidth(style::window::default_border_focucs_w);
        message->setPenWidth(style::window::messages::sms_border_no_focus);
        message->setFont(style::window::font::medium);
        message->setAlignment(Alignment(Alignment::ALIGN_HORIZONTAL_LEFT, Alignment::ALIGN_VERTICAL_BOTTOM));
        message->activatedCallback = [=](Item &) -> bool {
            auto app = dynamic_cast<app::ApplicationMessages *>(application);
            assert(app != nullptr);
            // if a valid contact was found, choose it. Otherwise, get a raw entered number
            UTF8 number =
                (contact && contact->numbers.size() != 0) ? contact->numbers[0].numberE164 : recipient->getText();
            auto ret = app->sendSms(number, message->getText());
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
                application->switchWindow(gui::name::window::thread_view,
                                          std::make_unique<SMSThreadData>(std::move(thread)));
            }
            else {
                LOG_FATAL("No thread and thread not created!");
                return false;
            }

            return true;
        };
        message->focusChangedCallback = [=](Item &) -> bool {
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_send"));
            return true;
        };
        body->addWidget(message);

        body->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        body->setVisible(true);
        body->setNavigation();
        setFocusItem(body);
    }

    void NewSMS_Window::destroyInterface()
    {
        AppWindow::destroyInterface();

        children.clear();
    }
}; // namespace gui
