// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhonebookSearchResults.hpp"
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookStyle.hpp"
#include "application-phonebook/widgets/PhonebookItem.hpp"

#include <service-db/QueryMessage.hpp>
#include <queries/phonebook/QueryContactGet.hpp>

namespace gui
{
    PhonebookSearchResults::PhonebookSearchResults(app::ApplicationCommon *app)
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
        bottomBar->setText(BottomBar::Side::RIGHT, utils::translate(style::strings::common::back));

        setTitle(utils::translate("common_results_prefix"));
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

    auto PhonebookSearchResults::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            return false;
        }

        auto searchResultsData = dynamic_cast<PhonebookSearchResultsData *>(data);
        assert(searchResultsData != nullptr);

        searchResultsModel = searchResultsData->consumeSearchResultsModel();
        setTitle(utils::translate("common_results_prefix") + "\"" + searchResultsModel->getFilter() + "\"");
        searchResultList->setProvider(searchResultsModel);

        if (searchResultsModel->messagesSelectCallback) {
            bottomBar->setActive(BottomBar::Side::LEFT, false);
            bottomBar->setText(BottomBar::Side::LEFT, "");
            bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::select));
        }
        else {
            bottomBar->setActive(BottomBar::Side::LEFT, true);
            bottomBar->setText(BottomBar::Side::LEFT, utils::translate(style::strings::common::call));
            bottomBar->setText(BottomBar::Side::CENTER, utils::translate(style::strings::common::open));
        }

        return true;
    }

} /* namespace gui */
