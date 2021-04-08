// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SearchResults.hpp"
#include "ThreadRecord.hpp"
#include <application-messages/data/SMSTextToSearch.hpp>
#include <application-messages/ApplicationMessages.hpp>
#include "module-apps/application-messages/data/MessagesStyle.hpp"
#include "queries/messages/threads/QueryThreadsSearchForList.hpp"

#include <service-db/QueryMessage.hpp>

#include <i18n/i18n.hpp>

namespace gui
{
    SearchResults::SearchResults(app::Application *app) : AppWindow(app, name::window::search_results)
    {
        namespace msgThreadStyle = style::messages::threads;

        AppWindow::buildInterface();
        setTitle(utils::localize.get("app_messages_title_main"));
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::search));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
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
                                 style::listview::ScrollBarType::Fixed);
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
