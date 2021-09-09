// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMessages.hpp"
#include "SMSTemplateItem.hpp"
#include "SMSTemplateModel.hpp"

#include <ListView.hpp>
#include <module-db/queries/messages/templates/QuerySMSTemplateGetForList.hpp>
#include <service-db/DBServiceAPI.hpp>

SMSTemplateModel::SMSTemplateModel(app::ApplicationCommon *app) : DatabaseModel(app), app::AsyncCallbackReceiver{app}
{}

unsigned int SMSTemplateModel::requestRecordsCount()
{
    return recordsCount;
}

bool SMSTemplateModel::updateRecords(std::vector<SMSTemplateRecord> records)
{
    if (DatabaseModel::updateRecords(std::move(records))) {
        list->onProviderDataUpdate();
        return true;
    }
    return false;
}

unsigned int SMSTemplateModel::getMinimalItemSpaceRequired() const
{
    return style::smsTemplItem::h;
}

gui::ListItem *SMSTemplateModel::getItem(gui::Order order)
{
    auto templ = getRecord(order);

    if (!templ) {
        return nullptr;
    }

    auto item = new gui::SMSTemplateItem();
    item->setTemplate(templ);
    item->activatedCallback = [=](gui::Item &it) {
        LOG_INFO("activatedCallback");
        if (auto app = dynamic_cast<app::ApplicationMessages *>(application)) {
            if (app->templatesCallback) {
                return app->templatesCallback(templ);
            }
        }
        return false;
    };

    return item;
}

void SMSTemplateModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    auto query = std::make_unique<db::query::SMSTemplateGetForList>(offset, limit);
    auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::SMSTemplate);
    task->setCallback([this](auto response) { return handleQueryResponse(response); });
    task->execute(application, this);
}

auto SMSTemplateModel::handleQueryResponse(db::QueryResult *queryResult) -> bool
{
    auto msgResponse = dynamic_cast<db::query::SMSTemplateGetForListResult *>(queryResult);
    assert(msgResponse != nullptr);

    // If list record count has changed we need to rebuild list.
    if (recordsCount != (msgResponse->getCount())) {
        recordsCount = msgResponse->getCount();
        list->reSendLastRebuildRequest();
        return false;
    }

    auto records = msgResponse->getResults();

    return this->updateRecords(std::move(records));
}
