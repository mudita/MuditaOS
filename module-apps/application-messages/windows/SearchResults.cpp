#include "SearchResults.hpp"
#include "ThreadRecord.hpp"
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
        list->setProvider(model.get());
        setFocusItem(list);
    }

    void SearchResults::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        if (mode == ShowMode::GUI_SHOW_INIT || data == nullptr) {
            model->clear();
            model->requestRecordsCount();
            list->clear();
            list->setElementsCount(model->getItemCount());
            setFocusItem(list);
        }
    }

    bool SearchResults::onDatabaseMessage(sys::Message *msgl)
    {
        if (auto msg = dynamic_cast<db::QueryResponse *>(msgl)) {
            if (auto response = dynamic_cast<db::query::SMSSearchResult *>(msg->getResult())) {
                int count         = response->getResults().size();
                auto records_data = response->getResults();
                auto records = std::make_unique<std::vector<ThreadRecord>>(records_data.begin(), records_data.end());
                model->updateRecords(std::move(records), 0, 3, count);
                model->setRecordsCount(response->getMax());
            }
            application->refreshWindow(gui::RefreshModes::GUI_REFRESH_FAST);
        }

        return false;
    }
}; // namespace gui
