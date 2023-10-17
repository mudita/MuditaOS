// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PhonebookSearchResults.hpp"
#include <Icon.hpp>
#include "application-phonebook/ApplicationPhonebook.hpp"
#include "application-phonebook/data/PhonebookStyle.hpp"

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
                                                      phonebookModel);
        searchResultList->setBoundaries(Boundaries::Continuous);
        setFocusItem(searchResultList);

        navBar->setActive(nav_bar::Side::Center, true);
        navBar->setActive(nav_bar::Side::Right, true);
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        icon = new Icon(this,
                        style::window::default_left_margin,
                        style::window::default_vertical_pos,
                        style::window::default_body_width,
                        style::window::default_body_height,
                        "",
                        "");

        icon->text->addRichText(utils::translate("app_phonebook_search_no_results"));
        icon->image->set("search_128px_W_G");
        icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        icon->image->setMargins(Margins(0, icon::image_top_margin, 0, icon::image_bottom_margin));
        icon->setVisible(false);

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
        // Model's data might have changed between calls
        if (mode == ShowMode::GUI_SHOW_RETURN) {
            handleContentUpdate();
        }
        searchResultList->rebuildList();
    }

    auto PhonebookSearchResults::handleSwitchData(SwitchData *data) -> bool
    {
        if (data == nullptr) {
            return false;
        }

        const auto searchResultsData = dynamic_cast<PhonebookSearchResultsData *>(data);
        assert(searchResultsData != nullptr);

        phonebookModel     = searchResultsData->phonebookModel;
        searchRequestModel = searchResultsData->searchRequestModel;
        searchResultList->setProvider(phonebookModel);
        handleContentUpdate();

        return true;
    }
    void PhonebookSearchResults::handleContentUpdate()
    {
        setTitle(utils::translate("common_results_prefix") + "\"" + phonebookModel->getFilter() + "\"");

        if (phonebookModel->requestRecordsCount() > 0) {
            navBar->setActive(nav_bar::Side::Left, true);
            navBar->setText(nav_bar::Side::Left, utils::translate(style::strings::common::call));
            navBar->setActive(nav_bar::Side::Center, true);
            if (searchRequestModel->requestedSearch()) {
                navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::select));
            }
            else {
                navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::open));
            }

            setFocusItem(searchResultList);
        }
        else {
            navBar->setActive(nav_bar::Side::Left, false);
            navBar->setActive(nav_bar::Side::Center, false);
            setFocusItem(icon);
        }
    }

} /* namespace gui */
