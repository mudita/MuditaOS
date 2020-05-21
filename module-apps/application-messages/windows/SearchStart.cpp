#include "SearchStart.hpp"
#include "../ApplicationMessages.hpp"
#include <i18/i18.hpp>
#include <widgets/SearchBox.hpp>
#include <cassert>

namespace gui
{

    SMSSearch::SMSSearch(app::Application *app) : AppWindow(app, name::window::thread_sms_search)
    {
        buildInterface();
        setTitle(utils::localize.get("app_messages_title_main"));
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::search));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        body = new gui::Item();
        body->setBoundingBox(bodySize());
        addWidget(body);
        inputCallback = [&](Item &, const InputEvent &inputEvent) -> bool {
            auto app = dynamic_cast<app::ApplicationMessages *>(application);
            assert(app);
            if (inputEvent.state != InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (inputEvent.keyCode == KeyCode::KEY_ENTER) {
                app->showSearchResults(utils::localize.get("app_phonebook_search_results"));
            }
            return false;
        };

        auto text = searchBox(this, utils::localize.get("app_phonebook_search_win_search"), "search");
        setFocusItem(text);
    }

    bool SMSSearch::onInput(const InputEvent &inputEvent)
    {
        return AppWindow::onInput(inputEvent) || (inputCallback && inputCallback(*this, inputEvent));
    }

} // namespace gui
