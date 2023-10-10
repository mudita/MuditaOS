// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationMessages.hpp"
#include "SMSTemplateItem.hpp"
#include "SMSTemplateModel.hpp"

#include <ListView.hpp>
#include <module-db/queries/messages/templates/QuerySMSTemplateGetForList.hpp>
#include <service-db/DBServiceAPI.hpp>
#include <AppWindow.hpp>

SMSTemplateModel::SMSTemplateModel(app::ApplicationCommon *app) : DatabaseModel(app), app::AsyncCallbackReceiver{app}
{}

unsigned int SMSTemplateModel::requestRecordsCount()
{
    return recordsCount;
}

bool SMSTemplateModel::updateRecords(std::vector<SMSTemplateRecord> records)
{
    DatabaseModel::updateRecords(std::move(records));
    list->onProviderDataUpdate();
    return true;
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

    if (recordsCount == msgResponse->getCount()) {
        return this->updateRecords(msgResponse->getResults());
    }

    recordsCount = msgResponse->getCount();
    if (list->isEmpty()) {
        list->clear();
    }

    // refreshWindow(...) does too few in this case
    const auto currentWindow = application->getCurrentWindow();
    application->switchWindow(currentWindow->getName());

    return false;
}
