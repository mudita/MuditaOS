
#include "SMSTemplateModel.hpp"
#include "SMSTemplateItem.hpp"
#include "application-messages/data/SMSdata.hpp"

#include <service-db/api/DBServiceAPI.hpp>

namespace config
{
    constexpr auto pageSize = 7;
}

SMSTemplateModel::SMSTemplateModel(app::Application *app) : DatabaseModel(app, config::pageSize)
{}

void SMSTemplateModel::requestRecordsCount()
{
    recordsCount = DBServiceAPI::CalllogGetCount(application);

    // request first and second page if possible
    if (recordsCount > 0) {
        LOG_INFO("DBServiceAPI::CalllogGetCount CalllogGetLimitOffset");
        DBServiceAPI::SMSTemplateGetLimitOffset(application, 0, pageSize);
        if (recordsCount >= pageSize) {
            DBServiceAPI::SMSTemplateGetLimitOffset(application, pageSize, pageSize);
        }
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

    DatabaseModel::updateRecords(std::move(records), offset, limit, count);

    return true;
}

gui::ListItem *SMSTemplateModel::getItem(
    int index, int firstElement, int prevElement, uint32_t count, int remaining, bool topDown)
{
    auto templ = getRecord(index);
    if (templ.get() == nullptr) {
        LOG_ERROR("getItem nullptr");
        return nullptr;
    }

    auto item = new gui::SMSTemplateItem(this);
    if (item != nullptr) {
        item->setTemplate(templ);
        item->setID(index);
        item->activatedCallback = [=](gui::Item &item) {
            LOG_INFO("activatedCallback");
            // std::unique_ptr<gui::SwitchData> data = std::make_unique<calllog::CallLogSwitchData>(*call);
            // application->switchWindow(calllog::settings::DetailsWindowStr, std::move(data));
            return true;
        };
        return item;
    }

    return nullptr;
}
