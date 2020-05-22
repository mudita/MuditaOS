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
    {}

    void SearchResultsModel::requestRecords(uint32_t offset, uint32_t limit)
    {
        if (std::string(search_value).compare("") != 0) {
            DBServiceAPI::GetByQuery(
                getApplication(),
                db::interface::SMSThread,
                std::make_unique<db::query::SMSSearch>(search_value, offset, max_search_items_on_screen));
        }
    }

    void SearchResultsModel::setRecordsCount(uint32_t count)
    {
        recordsCount = count;
    }

    void SearchResultsModel::setSearchValue(const UTF8 &search_value)
    {
        this->search_value = search_value;
    }

}; // namespace gui::model
