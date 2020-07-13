#include "PhonebookSearchResults.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookStyle.hpp"
#include "application-phonebook/widgets/PhonebookItem.hpp"
#include "application-phonebook/models/PhonebookModel.hpp"

#include <messages/QueryMessage.hpp>
#include <queries/phonebook/QueryContactGet.hpp>

#include <service-appmgr/ApplicationManager.hpp>

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
                                                      phonebookStyle::searchResults::searchResultList::x,
                                                      phonebookStyle::searchResults::searchResultList::y,
                                                      phonebookStyle::searchResults::searchResultList::w,
                                                      phonebookStyle::searchResults::searchResultList::h,
                                                      searchResultsModel);
        searchResultList->setPenFocusWidth(phonebookStyle::searchResults::searchResultList::penFocusWidth);
        searchResultList->setPenWidth(phonebookStyle::searchResults::searchResultList::penWidth);

        bottomBar->setActive(BottomBar::Side::LEFT, true);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get(style::strings::common::call));
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::open));
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
        setFocusItem(searchResultList);
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

        return true;
    }

    bool PhonebookSearchResults::onDatabaseMessage(sys::Message *msgl)
    {
        auto respMsg = dynamic_cast<sys::ResponseMessage *>(msgl);

        assert(respMsg != nullptr);
        assert(respMsg->responseTo == MessageType::DBQuery);

        auto queryResponse = dynamic_cast<db::QueryResponse *>(respMsg);
        if (queryResponse == nullptr) {
            LOG_ERROR("Unexpected message.");
            return false;
        }

        auto contactsResponse = dynamic_cast<db::query::ContactGetResult *>(queryResponse->getResult());
        assert(contactsResponse != nullptr);

        auto records = std::make_unique<std::vector<ContactRecord>>(contactsResponse->getRecords());

        return searchResultsModel->updateRecords(std::move(records));
    }
} /* namespace gui */
