
#include "SMSTemplateModel.hpp"
#include "SMSTemplateItem.hpp"
#include "application-messages/data/SMSdata.hpp"
#include "application-messages/MessagesStyle.hpp"
#include "application-messages/ApplicationMessages.hpp"
#include <service-db/api/DBServiceAPI.hpp>

SMSTemplateModel::SMSTemplateModel(app::Application *app) : DatabaseModel(app)
{}

void SMSTemplateModel::requestRecordsCount()
{
    recordsCount = DBServiceAPI::SMSTemplateGetCount(application);
    LOG_DEBUG("SMSTemplateGetCount %" PRIu32, recordsCount);
    // request first
    if (recordsCount > 0) {
        LOG_DEBUG("SMSTemplateGetLimitOffset");
        auto pageSize = style::messages::templates::list::pageSize;
        DBServiceAPI::SMSTemplateGetLimitOffset(application, 0, pageSize);
    }
}

void SMSTemplateModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    DBServiceAPI::SMSTemplateGetLimitOffset(application, offset, limit);
}

bool SMSTemplateModel::updateRecords(std::unique_ptr<std::vector<SMSTemplateRecord>> records,
                                     const uint32_t offset,
                                     const uint32_t limit,
                                     uint32_t count)
{

    LOG_INFO("Offset: %" PRIu32 ", Limit: %" PRIu32 " Count:%" PRIu32 "", offset, limit, count);

    if (DatabaseModel::updateRecords(std::move(records), offset, limit, count)) {
        list->onProviderDataUpdate();
        return true;
    }
    return false;
}

gui::ListItem *SMSTemplateModel::getItem(int index)
{
    auto templ = getRecord(index);
    if (!templ) {
        return nullptr;
    }

    auto item = new gui::SMSTemplateItem();
    item->setTemplate(templ);
    item->setID(index);
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
