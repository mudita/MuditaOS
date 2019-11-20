#include "PhonebookSearchResults.hpp"
#include "../ApplicationPhonebook.hpp"
#include "ContactRecord.hpp"
#include "Label.hpp"
#include "ListView.hpp"
#include "Margins.hpp"
#include "i18/i18.hpp"
#include "service-appmgr/ApplicationManager.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "service-db/messages/DBMessage.hpp"
#include <Style.hpp>
#include <functional>
#include <log/log.hpp>
#include <memory>

namespace gui
{

PhonebookSearchResults::PhonebookSearchResults(app::Application *app) : AppWindow(app, "SearchResults"), searchResultsModel{new SearchResultsModel(app)}
{
    setSize(style::window_width, style::window_height);
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

    list = new gui::PhonebookListView(this, 11, 105, 480 - 22, 600 - 105 - 50);
    list->setMaxElements(7);
    list->setPageSize(7);
    list->setPenFocusWidth(0);
    list->setPenWidth(0);
    list->setProvider(searchResultsModel);
    list->setApplication(application);

    bottomBar->setActive(BottomBar::Side::LEFT, true);
    bottomBar->setActive(BottomBar::Side::CENTER, true);
    bottomBar->setActive(BottomBar::Side::RIGHT, true);
    bottomBar->setText(BottomBar::Side::LEFT, utils::localize.get("app_phonebook_call"));
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("app_phonebook_open"));
    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

    topBar->setActive(TopBar::Elements::TIME, true);

    setTitle(utils::localize.get("app_phonebook_search_results_prefix"));

    rightArrowImage = new gui::Image(this, 480 - 30 - 13, 62, 0, 0, "arrow_right");
    searchImage = new gui::Image(this, 480 - 48 - 26, 55, 0, 0, "search");
}

void PhonebookSearchResults::destroyInterface()
{
    AppWindow::destroyInterface();
    if (list)
    {
        removeWidget(list);
        delete list;
        list = nullptr;
    }
    if (leftArrowImage)
    {
        removeWidget(leftArrowImage);
        delete leftArrowImage;
        leftArrowImage = nullptr;
    }
    if (rightArrowImage)
    {
        removeWidget(rightArrowImage);
        delete rightArrowImage;
        rightArrowImage = nullptr;
    }
    if (newContactImage)
    {
        removeWidget(newContactImage);
        delete newContactImage;
        newContactImage = nullptr;
    }
    if (searchImage)
    {
        removeWidget(searchImage);
        delete searchImage;
        searchImage = nullptr;
    }

    children.clear();
    delete searchResultsModel;
}

PhonebookSearchResults::~PhonebookSearchResults()
{
    destroyInterface();
}

void PhonebookSearchResults::onBeforeShow(ShowMode mode, SwitchData *data)
{
    setFocusItem(list);
}

bool PhonebookSearchResults::onInput(const InputEvent &inputEvent)
{
    // process only if key is released
    if (inputEvent.state == InputEvent::State::keyReleasedShort)
    {
        switch (inputEvent.keyCode)
        {
        case KeyCode::KEY_LEFT:
            LOG_INFO("Adding new contact");
            application->switchWindow("New");
            return true;
        case KeyCode::KEY_RIGHT:
            LOG_INFO("Searching contact");
            application->switchWindow("Search");
            return true;
        default:
            break;
        }
    }

    // check if any of the lower inheritance onInput methods catch the event
    return AppWindow::onInput(inputEvent);
}

bool PhonebookSearchResults::handleSwitchData(SwitchData *data)
{
    if (data == nullptr)
    {
        LOG_ERROR("Received null pointer");
        return false;
    }

    PhonebookSearchResultsData *searchResults = reinterpret_cast<PhonebookSearchResultsData *>(data);
    if (searchResults)
    {
        std::shared_ptr<std::vector<ContactRecord>> results = searchResults->getResults();
        LOG_INFO("display %d results", results->size());
        setTitle(utils::localize.get("app_phonebook_search_results_prefix") + "\"" + searchResults->getQuery() + "\"");
        searchResultsModel->setResults(results);
        list->clear();
        list->setElementsCount(searchResultsModel->getItemCount());
        return (true);
    }

    return (false);
}

} /* namespace gui */
