#include "SearchResults.hpp"
#include "ThreadRecord.hpp"
#include <application-messages/data/SMSTextToSearch.hpp>
#include <application-messages/ApplicationMessages.hpp>
#include "messages/DBThreadMessage.hpp"
#include "messages/QueryMessage.hpp"
#include "queries/sms/QuerySMSSearch.hpp"
#include "service-db/messages/DBMessage.hpp"
#include <i18/i18.hpp>

namespace gui
{
    SearchResults::SearchResults(app::Application *app) : AppWindow(app, name::window::search_results)
    {
        AppWindow::buildInterface();
        setTitle(utils::localize.get("app_messages_title_main"));
        bottomBar->setActive(BottomBar::Side::CENTER, true);
        bottomBar->setActive(BottomBar::Side::RIGHT, true);
        bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::search));
        bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));
        body = new gui::Item();
        body->setBoundingBox(bodySize());
        addWidget(body);

        model = std::make_unique<model::SearchResultsModel>(application);

        list = new gui::ListView(body,
                                 body->area().x + style::window::list_offset_default,
                                 8,
                                 body->area().w - 2 * style::window::list_offset_default,
                                 body->area().h);
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
            model->requestRecords(0, model->getMaxItemsOnScreen());
            list->setElementsCount(model->getItemCount());
            list->setProvider(model.get());
            setFocusItem(list);
            return true;
        }
        return false;
    }

    bool SearchResults::onDatabaseMessage(sys::Message *msgl)
    {
        if (auto msg = dynamic_cast<db::QueryResponse *>(msgl)) {
            if (auto response = dynamic_cast<db::query::SMSSearchResult *>(msg->getResult())) {
                return listViewUpdate(response);
            }
        }
        return false;
    }

    auto SearchResults::listViewUpdate(db::query::SMSSearchResult *response) -> bool
    {
        int count         = response->getResults().size();
        auto records_data = response->getResults();
        auto records      = std::make_unique<std::vector<ThreadRecord>>(records_data.begin(), records_data.end());
        model->updateRecords(std::move(records), 0, 3, count);
        model->setRecordsCount(response->getMax());
        if (response->getMax() == 0) {
            return showEmptyResults();
        }
        return true;
    }

    bool SearchResults::showEmptyResults()
    {
        auto app = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app);
        return app->searchEmpty();
    }
}; // namespace gui
