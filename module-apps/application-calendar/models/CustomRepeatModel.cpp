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

void CustomRepeatModel::createData()
{
    internalData.push_back(
        new gui::CheckBoxWithLabelItem(application, utils::localize.get(style::strings::common::Monday)));
    internalData.push_back(
        new gui::CheckBoxWithLabelItem(application, utils::localize.get(style::strings::common::Tuesday)));
    internalData.push_back(
        new gui::CheckBoxWithLabelItem(application, utils::localize.get(style::strings::common::Wednesday)));
    internalData.push_back(
        new gui::CheckBoxWithLabelItem(application, utils::localize.get(style::strings::common::Thursday)));
    internalData.push_back(
        new gui::CheckBoxWithLabelItem(application, utils::localize.get(style::strings::common::Friday)));
    internalData.push_back(
        new gui::CheckBoxWithLabelItem(application, utils::localize.get(style::strings::common::Saturday)));
    internalData.push_back(
        new gui::CheckBoxWithLabelItem(application, utils::localize.get(style::strings::common::Sunday)));

    for (auto &item : internalData) {
        item->deleteByList = false;
    }
}

void CustomRepeatModel::loadData()
{
    list->clear();
    eraseInternalData();

    createData();

    for (auto &item : internalData) {
        if (item->onLoadCallback) {
            item->onLoadCallback();
        }
    }

    list->rebuildList();
}
