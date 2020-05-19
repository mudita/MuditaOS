#include "SearchResultsModel.hpp"
#include "time/time_conversion.hpp"
#include "../widgets/SearchResultsItem.hpp"

#include "service-db/api/DBServiceAPI.hpp"
#include <module-db/queries/sms/QuerySMSSearch.hpp>

namespace gui::model
{

    SearchResultsModel::SearchResultsModel(app::Application *app) : BaseThreadRecordModel(app)
    {}

    gui::ListItem *SearchResultsModel::getItem(int index)
    {
        std::shared_ptr<ThreadRecord> thread = getRecord(index);
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
            ret->setPreview("this is preview text");
        }
        ret->setID(index);
        return ret;
    }

    void SearchResultsModel::requestRecordsCount()
    {
        // was used BaseThreadRecordModel::requestRecordsCount(void)
        // now I want to use more general API
        // virtual std::unique_ptr<std::vector<T>> GetByQuerry(std::unique_ptr<db::Query> querry)

        // consider && instead move instead
        DBServiceAPI::GetByQuery(
            getApplication(), db::DB::SMS, std::make_unique<db::query::SMSSearch>("what_to_serach", 0, 10));
        // recordsCount = result_from_QuerySMS_Search()->max_depth;
    }

}; // namespace gui::model
