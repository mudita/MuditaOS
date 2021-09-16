// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewEditAlarmModel.hpp"
#include "application-alarm-clock/widgets/AlarmTimeItem.hpp"
#include "application-alarm-clock/widgets/AlarmOptionsItem.hpp"
#include "application-alarm-clock/widgets/addedit/AlarmOptionRepeat.hpp"
#include "application-alarm-clock/widgets/AlarmClockStyle.hpp"
#include <ListView.hpp>

namespace app::alarmClock
{
    NewEditAlarmModel::NewEditAlarmModel(app::ApplicationCommon *app,
                                         std::shared_ptr<AbstractAlarmsRepository> alarmsRepository,
                                         bool mode24H)
        : application(app), alarmsRepository{std::move(alarmsRepository)}, mode24H(mode24H)
    {}

    unsigned int NewEditAlarmModel::requestRecordsCount()
    {
        return internalData.size();
    }

    unsigned int NewEditAlarmModel::getMinimalItemSpaceRequired() const
    {
        return style::alarmClock::window::item::options::height;
    }

    void NewEditAlarmModel::requestRecords(uint32_t offset, uint32_t limit)
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    gui::ListItem *NewEditAlarmModel::getItem(gui::Order order)
    {
        return getRecord(order);
    }

    void NewEditAlarmModel::createData(std::shared_ptr<AlarmEventRecord> record)
    {
        auto app = application;
        assert(app != nullptr);

        internalData.push_back(new gui::AlarmTimeItem(
            mode24H,
            [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
            [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); }));
        internalData.push_back(new gui::AlarmOptionsItem(
            application,
            AlarmOptionItemName::Sound,
            [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
            [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); }));

        internalData.push_back(new gui::AlarmOptionsItem(
            application,
            AlarmOptionItemName::Snooze,
            [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
            [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); }));

        internalData.push_back(new gui::AlarmOptionRepeat(
            application,
            AlarmOptionItemName::Repeat,
            AlarmPresenter(record),
            [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
            [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); }));

        for (auto &item : internalData) {
            item->deleteByList = false;
        }
    }

    void NewEditAlarmModel::loadData(std::shared_ptr<AlarmEventRecord> record)
    {
        list->reset();
        eraseInternalData();

        createData(record);

        for (auto &item : internalData) {
            if (item->onLoadCallback) {
                item->onLoadCallback(record);
            }
        }

        list->rebuildList();
    }

    void NewEditAlarmModel::loadRepeat(std::shared_ptr<AlarmEventRecord> record)
    {
        if (repeatOption->onLoadCallback) {
            repeatOption->onLoadCallback(std::move(record));
        }
    }

    void NewEditAlarmModel::saveData(std::shared_ptr<AlarmEventRecord> alarm, AlarmAction action)
    {
        for (auto &item : internalData) {
            if (item->onSaveCallback) {
                item->onSaveCallback(alarm);
            }
        }

        if (action == AlarmAction::Edit) {
            alarmsRepository->update(*alarm,
                                     [this](bool) { application->switchWindow(gui::name::window::main_window); });
        }
        else {
            alarmsRepository->add(*alarm, [this](bool) { application->returnToPreviousWindow(); });
        }
        list->reset();
        eraseInternalData();
    }
} // namespace app::alarmClock
