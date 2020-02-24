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

#include "../widgets/PhonebookItem.hpp"

namespace gui
{

    PhonebookSearchResults::PhonebookSearchResults(app::Application *app)
        : AppWindow(app, name::window::search_results), searchResultsModel{new SearchResultsModel(app)}
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
    bool ret = false;
    if(AppWindow::onInput(inputEvent))
    {
        return true;
    }

    // process only if key is released
    if ((inputEvent.state != InputEvent::State::keyReleasedShort) && ((inputEvent.state != InputEvent::State::keyReleasedLong)))
        return false;

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
    return ret;
}

bool PhonebookSearchResults::handleSwitchData(SwitchData *data)
{
    if (data == nullptr)
        return false;

    auto fill_results = [=](std::shared_ptr<std::vector<ContactRecord>> res, const std::string &title) {
        if (res == nullptr || res->size() == 0)
        {
            return;
        }
        searchResultsModel->setResults(res);
        list->clear();
        list->setElementsCount(res.get()->size());
        setTitle(utils::localize.get("app_phonebook_search_results_prefix") + "\"" + title + "\"");
    };

    auto searchResults = dynamic_cast<PhonebookSearchResultsData *>(data);
    if (searchResults)
    {
        fill_results(searchResults->getResults(), searchResults->getQuery());
        return (true);
    }

    auto contactRequest = dynamic_cast<PhonebookSearchReuqest *>(data);
    if (contactRequest)
    {
        list->cb_ENTER = [=](gui::PhonebookItem *item) {
            std::unique_ptr<PhonebookSearchReuqest> data = std::make_unique<PhonebookSearchReuqest>();
            data->result = item->getContact();
            data->setDescription("PhonebookSearchRequest");
            return sapm::ApplicationManager::messageSwitchPreviousApplication(
                application, std::make_unique<sapm::APMSwitchPrevApp>(application->GetName(), std::move(data)));
        };
        fill_results(contactRequest->results, contactRequest->request);
    }

    return (false);
}

} /* namespace gui */
