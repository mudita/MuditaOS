#include "PhonebookSearchResults.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookStyle.hpp"
#include "application-phonebook/widgets/PhonebookItem.hpp"

#include <messages/QueryMessage.hpp>
#include <queries/phonebook/QueryContactGet.hpp>

namespace gui
{
    PhonebookSearchResults::PhonebookSearchResults(app::Application *app)
        : AppWindow(app, gui::window::name::search_results)
    {
        buildInterface();
    }

    void PhonebookSearchResults::rebuild()
    {
        destroyInterface();
        buildInterface();
    }

    void PhonebookSearchResults::buildInterface()
    {

        AppWindow::buildInterface();

        searchResultList = new gui::PhonebookListView(this,
                                                      phonebookStyle::searchResultsWindow::searchResultList::x,
                                                      phonebookStyle::searchResultsWindow::searchResultList::y,
                                                      phonebookStyle::searchResultsWindow::searchResultList::w,
                                                      phonebookStyle::searchResultsWindow::searchResultList::h,
                                                      searchResultsModel);
        setFocusItem(searchResultList);

        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

        topBar->setActive(TopBar::Elements::TIME, true);

        setTitle(utils::localize.get("common_results_prefix"));
    }

    void PhonebookSearchResults::destroyInterface()
    {
        erase();
    }

    PhonebookSearchResults::~PhonebookSearchResults()
    {
        destroyInterface();
    }

    void PhonebookSearchResults::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_INIT) {
            searchResultList->rebuildList();
        }
    }

    auto PhonebookSearchResults::onInput(const InputEvent &inputEvent) -> bool
    {
        if (AppWindow::onInput(inputEvent)) {
            return true;
        }

        // process only if key is released
        if (inputEvent.state != InputEvent::State::keyReleasedShort) {
            return false;
        }
        if (inputEvent.state == InputEvent::State::keyReleasedShort) {
            switch (inputEvent.keyCode) {
            case KeyCode::KEY_LEFT:
                LOG_INFO("Adding new contact");
                application->switchWindow(gui::window::name::new_contact);
                return true;
            case KeyCode::KEY_RIGHT:
                LOG_INFO("Searching contact");
                application->switchWindow("Search");
                return true;
            default:
                break;
            }
        }

        return false;
    }

    auto PhonebookSearchResults::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            return false;
        }

        auto searchResultsData = dynamic_cast<PhonebookSearchResultsData *>(data);
        assert(searchResultsData != nullptr);

        searchResultsModel = searchResultsData->consumeSearchResultsModel();
        setTitle(utils::localize.get("common_results_prefix") + "\"" + searchResultsModel->getFilter() + "\"");
        searchResultList->setProvider(searchResultsModel);

        if (searchResultsModel->messagesSelectCallback) {
            bottomBar->setActive(BottomBar::Side::LEFT, false);
            bottomBar->setText(BottomBar::Side::LEFT, "");
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::select));
        }
        else {
            bottomBar->setActive(BottomBar::Side::LEFT, true);
            bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get(style::strings::common::call));
            bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));
        }

        return true;
    }

} /* namespace gui */
