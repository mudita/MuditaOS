// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMessages.hpp"
#include "MessagesStyle.hpp"
#include "SearchResultsItem.hpp"
#include "ThreadsSearchResultsModel.hpp"

#include <ListView.hpp>
#include <module-db/queries/messages/threads/QueryThreadsSearchForList.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <time/time_conversion_factory.hpp>

namespace gui::model
{

    ThreadsSearchResultsModel::ThreadsSearchResultsModel(app::Application *app)
        : BaseThreadsRecordModel(app), app::AsyncCallbackReceiver{app}
    {}

    auto ThreadsSearchResultsModel::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::messages::threadItem::sms_thread_item_h;
    }

    auto ThreadsSearchResultsModel::getItem(gui::Order order) -> gui::ListItem *
    {
        std::shared_ptr<ThreadListStruct> threadStruct = getRecord(order);

        if (threadStruct == nullptr) {
            return nullptr;
        }

        auto ret = new gui::SearchResultsItem();
        {
            using namespace utils::time;
            ret->setContact(threadStruct->contact->getFormattedName());
            ret->setTimestamp(*TimestampFactory().createTimestamp(TimestampType::DateTime, threadStruct->thread->date));
            // The only thing that differs with ThreadModel actually - here show what was found
            ret->setPreview(threadStruct->thread->snippet);
        }

        return ret;
    }

    void ThreadsSearchResultsModel::requestRecords(uint32_t offset, uint32_t limit)
    {
        if (std::string(textToSearch).compare("") != 0) {
            auto query = std::make_unique<db::query::ThreadsSearchForList>(textToSearch, offset, limit);
            auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::SMSThread);
            task->setCallback([this](auto response) { return handleQueryResponse(response); });
            task->execute(application, this);
        }
    }

    void ThreadsSearchResultsModel::setSearchValue(const UTF8 &value)
    {
        this->textToSearch = value;
    }

    auto ThreadsSearchResultsModel::handleQueryResponse(db::QueryResult *queryResult) -> bool
    {
        auto msgResponse = dynamic_cast<db::query::ThreadsSearchResultForList *>(queryResult);
        assert(msgResponse != nullptr);

        // If list record count has changed we need to rebuild list.
        if (recordsCount != (msgResponse->getCount())) {
            recordsCount = msgResponse->getCount();
            list->reSendLastRebuildRequest();
            return false;
        }

        auto threads  = msgResponse->getResults();
        auto contacts = msgResponse->getContacts();

        std::vector<ThreadListStruct> records;

        assert(threads.size() == contacts.size());

        for (unsigned int i = 0; i < threads.size(); i++) {
            records.emplace_back(
                std::make_shared<ThreadRecord>(threads[i]), std::make_shared<ContactRecord>(contacts[i]), nullptr);
        }

        return this->updateRecords(std::move(records));
    }

}; // namespace gui::model
