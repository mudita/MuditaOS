#include "PhonebookSearch.hpp"
#include "../data/PhonebookStyle.hpp"
#include "Utils.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "widgets/SearchBox.hpp"

namespace gui
{
    PhonebookSearch::PhonebookSearch(app::Application *app)
        : AppWindow(app, "Search"), phonebookModel{new PhonebookModel(app)}
    {
        buildInterface();
    }

    void PhonebookSearch::buildInterface()
    {
        AppWindow::buildInterface();
        topBar->setActive(TopBar::Elements::TIME, true);

        setTitle(utils::localize.get("app_phonebook_title_main"));

        inputField = searchBox(this, utils::localize.get("app_phonebook_search_win_search"), "search");
        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_search"));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get("app_phonebook_back"));

        setFocusItem(inputField);
    }

    void PhonebookSearch::rebuild()
    {}

    void PhonebookSearch::destroyInterface()
    {
        AppWindow::destroyInterface();
    }

    void PhonebookSearch::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        setFocusItem(inputField);
    }

    bool PhonebookSearch::handleSwitchData(SwitchData *data)
    {
        if (data == nullptr) {
            LOG_ERROR("Received null pointer");
            return false;
        }

        PhonebookSearchQuery *item = dynamic_cast<PhonebookSearchQuery *>(data);
        inputField->setText(item->getQuery());

        return (true);
    }

    bool PhonebookSearch::onInput(const InputEvent &inputEvent)
    {
        bool ret = AppWindow::onInput(inputEvent);
        if (ret) {
            return ret;
        }

        if ((inputEvent.state != InputEvent::State::keyReleasedShort) &&
            ((inputEvent.state != InputEvent::State::keyReleasedLong))) {
            return (false);
        }

        if (inputEvent.keyCode == KeyCode::KEY_ENTER) {
            LOG_INFO("Enter pressed");
            std::string contents = utils::trim(inputField->getText());
            std::shared_ptr<std::vector<ContactRecord>> searchResults;
            std::string primaryName     = "";
            std::string alternativeName = "";
            std::string number          = "";

            if (contents.size() > 0) {
                if (utils::is_number(contents) || contents.find('+') != std::string::npos) {
                    number = contents;
                }
                else if (contents.find(' ') != std::string::npos) {
                    std::vector<std::string> result = utils::split(contents, ' ');
                    primaryName                     = result[0];
                    alternativeName                 = result[1];
                }
                else {
                    primaryName = contents;
                }

                LOG_INFO(
                    "searching for pri:%s alt:%s num:%s", primaryName.c_str(), alternativeName.c_str(), number.c_str());
                searchResults = DBServiceAPI::ContactSearch(application, primaryName, alternativeName, number);
                if (searchResults.get()->size() > 0) {
                    std::unique_ptr<PhonebookSearchResultsData> data =
                        std::make_unique<PhonebookSearchResultsData>(inputField->getText());
                    data->setResults(searchResults);
                    application->switchWindow("SearchResults", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                }
                else {
                    std::unique_ptr<gui::SwitchData> data =
                        std::make_unique<PhonebookSearchQuery>(inputField->getText());
                    application->switchWindow("NoResults", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                }
                return (true);
            }
        }

        return (ret);
    }
} /* namespace gui */
