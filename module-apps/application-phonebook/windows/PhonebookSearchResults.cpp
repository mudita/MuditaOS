// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
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
        searchResultList->setBoundaries(Boundaries::Continuous);
        setFocusItem(searchResultList);

        navBar->setActive(nav_bar::Side::Center, true);
        navBar->setActive(nav_bar::Side::Right, true);
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

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

        if (searchResultsModel->contactSelectCallback) {
            navBar->setActive(nav_bar::Side::Left, false);
            navBar->setText(nav_bar::Side::Left, "");
            navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::select));
        }
        else {
            navBar->setActive(nav_bar::Side::Left, true);
            navBar->setText(nav_bar::Side::Left, utils::translate(style::strings::common::call));
            navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::open));
        }

        return true;
    }

} /* namespace gui */
