#include "ThreadsSearchResultsModel.hpp"
#include "ListView.hpp"
#include "time/time_conversion.hpp"
#include "application-messages/widgets/SearchResultsItem.hpp"

#include "service-db/api/DBServiceAPI.hpp"
#include <module-db/queries/sms/QuerySMSSearch.hpp>
#include <module-apps/application-messages/ApplicationMessages.hpp>

namespace gui::model
{

    ThreadsSearchResultsModel::ThreadsSearchResultsModel(app::Application *app) : BaseThreadsRecordModel(app)
    {}

    unsigned int ThreadsSearchResultsModel::getMinimalItemHeight() const
    {
        return style::window::messages::sms_thread_item_h;
    }

    gui::ListItem *ThreadsSearchResultsModel::getItem(gui::Order order)
    {

        std::shared_ptr<ThreadRecord> thread = getRecord(order);

        if (thread.get() == nullptr) {
            return nullptr;
        }

        auto ret = new gui::SearchResultsItem();
        {
            auto contactRec = DBServiceAPI::ContactGetByID(getApplication(), thread->contactID);
            auto cont       = contactRec->front();
            ret->setContact(cont.getFormattedName());
        }
        ret->setTimestamp(utils::time::DateTime(thread->date));
        {
            // The only thing that differs with ThreadModel actually - here show what was found
            ret->setPreview(thread->snippet);
        }

        return ret;
    }

    void ThreadsSearchResultsModel::requestRecords(uint32_t offset, uint32_t limit)
    {
        if (std::string(search_value).compare("") != 0) {
            auto query = std::make_unique<db::query::SMSSearch>(search_value, offset, limit);
            query->setQueryListener(this);
            DBServiceAPI::GetQuery(getApplication(), db::Interface::Name::SMSThread, std::move(query));
        }
    }

    void ThreadsSearchResultsModel::setSearchValue(const UTF8 &search_value)
    {
        this->search_value = search_value;
    }

    auto ThreadsSearchResultsModel::handleQueryResponse(db::QueryResult *queryResult) -> bool
    {
        auto msgResponse = dynamic_cast<db::query::SMSSearchResult *>(queryResult);
        assert(msgResponse != nullptr);

        auto records_data = msgResponse->getResults();
        auto records      = std::make_unique<std::vector<ThreadRecord>>(records_data.begin(), records_data.end());

        if (msgResponse->getMax() == 0) {

            auto app = dynamic_cast<app::ApplicationMessages *>(application);
            assert(app);
            return app->searchEmpty();
        }

        assert(list);
        list->setElementsCount(msgResponse->getMax());

        return this->updateRecords(std::move(records));
    }

}; // namespace gui::model
