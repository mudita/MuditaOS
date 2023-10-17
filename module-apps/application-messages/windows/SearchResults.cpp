// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMessages.hpp"
#include "MessagesStyle.hpp"
#include "SearchResults.hpp"
#include "SMSTextToSearch.hpp"
#include "ThreadRecord.hpp"

#include <i18n/i18n.hpp>
#include <queries/messages/threads/QueryThreadsSearchForList.hpp>
#include <service-db/QueryMessage.hpp>

namespace gui
{
    SearchResults::SearchResults(app::ApplicationCommon *app) : AppWindow(app, name::window::search_results)
    {
        namespace msgThreadStyle = style::messages::threads;

        AppWindow::buildInterface();
        setTitle(utils::translate("app_messages_title_main"));
        navBar->setActive(nav_bar::Side::Center, true);
        navBar->setActive(nav_bar::Side::Right, true);
        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::search));
        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
        body = new gui::Item();
        body->setBoundingBox(bodySize());
        addWidget(body);

        model = std::make_shared<model::ThreadsSearchResultsModel>(application);

        list = new gui::ListView(this,
                                 msgThreadStyle::listPositionX,
                                 msgThreadStyle::ListPositionY,
                                 msgThreadStyle::listWidth,
                                 msgThreadStyle::listHeight,
                                 model,
                                 listview::ScrollBarType::Fixed);
        list->setScrollTopMargin(style::margins::small);
        setFocusItem(list);

        list->emptyListCallback = [this]() { showEmptyResults(); };
    }

    void SearchResults::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_INIT && data != nullptr) {
            if (auto search_data = dynamic_cast<SMSTextToSearch *>(data)) {
                if (listViewRequest(search_data->getTextToSearch()) == false) {
                    showEmptyResults();
                }
            }
        }
    }

    bool SearchResults::listViewRequest(const std::string &text)
    {
        if (text.length() != 0u) {
            model->setSearchValue(text);
            list->rebuildList();
            setFocusItem(list);
            return true;
        }
        return false;
    }

    bool SearchResults::showEmptyResults()
    {
        auto app = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app);
        return app->searchEmpty();
    }
}; // namespace gui
