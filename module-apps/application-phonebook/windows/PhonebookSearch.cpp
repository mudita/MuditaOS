#include "PhonebookSearch.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "../data/PhonebookStyle.hpp"
#include "Utils.hpp"
#include "service-db/api/DBServiceAPI.hpp"
#include "widgets/SearchBox.hpp"

namespace gui
{
    PhonebookSearch::PhonebookSearch(app::Application *app) : AppWindow(app, gui::window::name::search)
    {
        buildInterface();
    }

    void PhonebookSearch::buildInterface()
    {
        AppWindow::buildInterface();
        topBar->setActive(TopBar::Elements::TIME, true);

        setTitle(utils::localize.get("app_phonebook_title_main"));

        inputField = searchBox(this, utils::localize.get("common_search_uc"), "search");
        bottomBar->setActive(BottomBar::Side::LEFT, false);
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);

        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::search));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

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
            std::string searchFilter = utils::trim(inputField->getText());

            if (searchFilter.size() > 0) {
                auto searchModel = std::make_unique<PhonebookModel>(application, searchFilter);

                assert(searchModel->getItemCount() >= 0);
                LOG_DEBUG("Search results count: %u", searchModel->getItemCount());
                if (searchModel->getItemCount() > 0) {
                    LOG_DEBUG("Switching to search results window.");
                    auto data = std::make_unique<PhonebookSearchResultsData>(std::move(searchModel));
                    application->switchWindow("SearchResults", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                }
                else {
                    LOG_DEBUG("Switching to no results window.");
                    std::unique_ptr<gui::SwitchData> data =
                        std::make_unique<PhonebookSearchQuery>(inputField->getText());
                    application->switchWindow("NoResults", gui::ShowMode::GUI_SHOW_INIT, std::move(data));
                }

                return true;
            }
        }

        return (ret);
    } // namespace gui
} /* namespace gui */
