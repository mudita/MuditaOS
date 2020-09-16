#include "SearchResults.hpp"
#include "ThreadRecord.hpp"
#include <application-messages/data/SMSTextToSearch.hpp>
#include <application-messages/ApplicationMessages.hpp>
#include "module-apps/application-messages/data/MessagesStyle.hpp"
#include "messages/DBThreadMessage.hpp"
#include "messages/QueryMessage.hpp"
#include "queries/sms/QuerySMSSearch.hpp"
#include "service-db/messages/DBMessage.hpp"
#include <i18/i18.hpp>

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
                                 model);
        list->setScrollTopMargin(style::margins::small);
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
