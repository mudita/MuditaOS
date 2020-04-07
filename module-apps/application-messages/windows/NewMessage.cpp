#include "NewMessage.hpp"
#include "ThreadViewWindow.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/windows/PhonebookSearchResults.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include <i18/i18.hpp>
#include <widgets/BoxLayout.hpp>
#include <widgets/Text.hpp>

namespace gui
{
    NewSMS_Window::NewSMS_Window(app::Application *app) : AppWindow(app, name::window::new_sms)
    {
        AppWindow::buildInterface();
        bottomBar->setActive(BottomBar::Side::LEFT, true);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("common_options"));
        bottomBar->setText(BottomBar::Side::CENTER, "SELECT");
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("common_back"));
        setTitle(utils::localize.get("sms_title_message"));

        uint32_t w = this->getWidth() - style::window::default_left_margin * 2;
        uint32_t h = this->getHeight() - title->offset_h() - bottomBar->getHeight();
        auto body  = new gui::VBox(this, style::window::default_left_margin, (uint32_t)title->offset_h(), w, h);

        auto label = new Label(body, 0, 0, body->getWidth(), 40);
        label->setText(utils::localize.get("sms_add_rec_num"));
        label->activeItem = false;
        label->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);

        auto text = new gui::Text(nullptr, 0, 0, body->getWidth(), 40, "", gui::Text::ExpandMode::EXPAND_UP);
        text->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
        text->setInputMode(new InputMode({InputMode::digit, InputMode::ABC, InputMode::abc}));
        text->setPenFocusWidth(style::window::default_border_focucs_w);
        text->activatedCallback = [=](Item &) -> bool {
            std::shared_ptr<std::vector<ContactRecord>> searchResults =
                DBServiceAPI::ContactSearch(application, text->getText(), text->getText(), text->getText());
            LOG_INFO("Get contact from another app, contacts matching num: %d",
                     static_cast<int>(searchResults.get()->size()));
            if (searchResults.get()->size() > 0) {
                std::unique_ptr<PhonebookSearchReuqest> data =
                    std::make_unique<PhonebookSearchReuqest>(text->getText(), searchResults);
                return sapm::ApplicationManager::messageSwitchApplication(
                    application, app::name_phonebook, name::window::search_results, std::move(data));
            }
            return true;
        };

        body->setEdges(gui::RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        body->addWidget(text);
        body->setVisible(true);
        body->setNavigation();
        setFocusItem(body);
    }

    bool NewSMS_Window::onInput(const InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent);
    }

    void NewSMS_Window::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (dynamic_cast<PhonebookSearchReuqest *>(data)) {
            LOG_INFO("SUCCESS!");
            application->switchWindow(gui::name::window::thread_view, std::make_unique<SwitchData>(*data));
        }
    }

    void NewSMS_Window::rebuild()
    {}

    void NewSMS_Window::buildInterface()
    {}

    void NewSMS_Window::destroyInterface()
    {}
}; // namespace gui
