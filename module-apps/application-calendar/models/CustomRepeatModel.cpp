#include "CustomRepeatModel.hpp"
#include "application-calendar/widgets/CheckBoxWithLabelItem.hpp"
#include <ListView.hpp>
#include <Utils.hpp>

CustomRepeatModel::CustomRepeatModel(app::Application *app) : application(app)
{}

unsigned int CustomRepeatModel::requestRecordsCount()
{
    return internalData.size();
}

unsigned int CustomRepeatModel::getMinimalItemHeight() const
{
    return style::window::calendar::item::checkBox::height;
}

void CustomRepeatModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

gui::ListItem *CustomRepeatModel::getItem(gui::Order order)
{
    return getRecord(order);
}

void CustomRepeatModel::createData(const std::shared_ptr<WeekDaysRepeatData> &data)
{
    internalData.push_back(
        new gui::CheckBoxWithLabelItem(application, utils::localize.get(style::strings::common::Monday), data));
    internalData.push_back(
        new gui::CheckBoxWithLabelItem(application, utils::localize.get(style::strings::common::Tuesday), data));
    internalData.push_back(
        new gui::CheckBoxWithLabelItem(application, utils::localize.get(style::strings::common::Wednesday), data));
    internalData.push_back(
        new gui::CheckBoxWithLabelItem(application, utils::localize.get(style::strings::common::Thursday), data));
    internalData.push_back(
        new gui::CheckBoxWithLabelItem(application, utils::localize.get(style::strings::common::Friday), data));
    internalData.push_back(
        new gui::CheckBoxWithLabelItem(application, utils::localize.get(style::strings::common::Saturday), data));
    internalData.push_back(
        new gui::CheckBoxWithLabelItem(application, utils::localize.get(style::strings::common::Sunday), data));

    for (auto &item : internalData) {
        item->deleteByList = false;
    }
}

void CustomRepeatModel::loadData(const std::shared_ptr<WeekDaysRepeatData> &data)
{
    list->clear();
    eraseInternalData();

    createData(data);

    list->rebuildList();
}
