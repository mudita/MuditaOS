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
            ret->setPreview(thread->snippet);
        }
        ret->setID(index);
        return ret;
    }

    void SearchResultsModel::requestRecordsCount()
    {
        DBServiceAPI::GetByQuery(
            getApplication(), db::interface::SMSThread, std::make_unique<db::query::SMSSearch>("naj", 0, 10));
    }

    void SearchResultsModel::setRecordsCount(uint32_t count)
    {
        recordsCount = count;
    }

}; // namespace gui::model
