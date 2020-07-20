#include "NewEditEventModel.hpp"
#include "application-calendar/widgets/CheckBoxWithLabelAndModel.hpp"
#include <ListView.hpp>
#include <Utils.hpp>
#include <BottomBar.hpp>

NewEditEventModel::NewEditEventModel(app::Application *app, bool mode24H) : application(app), mode24H(mode24H)
{}

unsigned int NewEditEventModel::requestRecordsCount()
{
    return internalData.size();
}

unsigned int NewEditEventModel::getMinimalItemHeight() const
{
    return style::window::calendar::item::checkBox::height;
}

void NewEditEventModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

gui::ListItem *NewEditEventModel::getItem(gui::Order order)
{
    return getRecord(order);
}

void NewEditEventModel::createData(bool allDayEvent)
{
    auto app = application;
    assert(app != nullptr);

    eventNameInput = new gui::TextWithLabelItem(
        utils::localize.get("app_calendar_new_edit_event_name"),
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); },
        [app]() { app->getCurrentWindow()->selectSpecialCharacter(); });

    allDayEventCheckBox = new gui::CheckBoxWithLabelAndModel(
        application, utils::localize.get("app_calendar_new_edit_event_allday"), true, this);

    startTime = new gui::EventTimeItem(
        utils::localize.get("app_calendar_new_edit_event_start"),
        mode24H,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    endTime = new gui::EventTimeItem(
        utils::localize.get("app_calendar_new_edit_event_end"),
        mode24H,
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    reminder = new gui::SeveralOptionsItem(
        application,
        utils::localize.get("app_calendar_event_detail_reminder"),
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    repeat = new gui::SeveralOptionsItem(
        application,
        utils::localize.get("app_calendar_event_detail_repeat"),
        [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
        [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });

    internalData.push_back(eventNameInput);
    internalData.push_back(allDayEventCheckBox);
    if (!allDayEvent) {
        internalData.push_back(startTime);
        internalData.push_back(endTime);
    }
    internalData.push_back(reminder);
    internalData.push_back(repeat);

    for (auto &item : internalData) {
        item->deleteByList = false;
    }
}

void NewEditEventModel::loadData(bool allDayEvent)
{
    list->clear();
    eraseInternalData();

    createData(allDayEvent);

    for (auto &item : internalData) {
        if (item->onLoadCallback) {
            item->onLoadCallback();
        }
    }

    list->rebuildList();
}

void NewEditEventModel::loadDataWithoutTimeItem()
{
    internalData.erase(std::find(internalData.begin(), internalData.end(), startTime));
    internalData.erase(std::find(internalData.begin(), internalData.end(), endTime));
    list->rebuildList();
}

void NewEditEventModel::reloadDataWithTimeItem()
{
    [[maybe_unused]] auto app = application;
    assert(app != nullptr);

    internalData.clear();

    internalData.push_back(eventNameInput);
    internalData.push_back(allDayEventCheckBox);
    internalData.push_back(startTime);
    internalData.push_back(endTime);
    internalData.push_back(reminder);
    internalData.push_back(repeat);

    for (auto &item : internalData) {
        item->deleteByList = false;
    }

    list->rebuildList();
}

void NewEditEventModel::saveData()
{
    for (auto &item : internalData) {
        if (item->onSaveCallback) {
            item->onSaveCallback();
        }
    }
}
