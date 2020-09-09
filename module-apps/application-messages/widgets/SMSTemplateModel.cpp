
#include "SMSTemplateModel.hpp"
#include "SMSTemplateItem.hpp"
#include "ListView.hpp"
#include "application-messages/data/SMSdata.hpp"
#include "application-messages/data/MessagesStyle.hpp"
#include "application-messages/ApplicationMessages.hpp"
#include <service-db/api/DBServiceAPI.hpp>

SMSTemplateModel::SMSTemplateModel(app::Application *app) : DatabaseModel(app)
{}

unsigned int SMSTemplateModel::requestRecordsCount()
{
    recordsCount = DBServiceAPI::SMSTemplateGetCount(application);
    LOG_DEBUG("SMSTemplateGetCount %" PRIu32, recordsCount);

    return recordsCount;
}

void SMSTemplateModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    DBServiceAPI::SMSTemplateGetLimitOffset(application, offset, limit);
}

bool SMSTemplateModel::updateRecords(std::unique_ptr<std::vector<SMSTemplateRecord>> records)
{
    if (DatabaseModel::updateRecords(std::move(records))) {
        list->onProviderDataUpdate();
        return true;
    }
    return false;
}

unsigned int SMSTemplateModel::getMinimalItemHeight() const
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
