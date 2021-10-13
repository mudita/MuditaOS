// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NewEditAlarmModel.hpp"

#include <application-alarm-clock/widgets/AlarmTimeItem.hpp>
#include <application-alarm-clock/widgets/AlarmSnoozeOptionsItem.hpp>
#include <application-alarm-clock/widgets/AlarmMusicOptionsItem.hpp>
#include <application-alarm-clock/widgets/AlarmRRuleOptionsItem.hpp>
#include <application-alarm-clock/presenter/AlarmRRulePresenter.hpp>
#include <application-alarm-clock/widgets/AlarmClockStyle.hpp>

#include <ListView.hpp>

namespace app::alarmClock
{
    NewEditAlarmModel::NewEditAlarmModel(app::ApplicationCommon *app,
                                         std::shared_ptr<SoundsPlayer> player,
                                         std::shared_ptr<alarmClock::AlarmRRulePresenter> rRulePresenter,
                                         std::shared_ptr<AbstractAlarmsRepository> alarmsRepository)
        : application(app), alarmsRepository{std::move(alarmsRepository)}, soundsPlayer(player),
          rRulePresenter(rRulePresenter)
    {}

    unsigned int NewEditAlarmModel::requestRecordsCount()
    {
        return internalData.size();
    }

    unsigned int NewEditAlarmModel::getMinimalItemSpaceRequired() const
    {
        return style::alarmClock::window::item::options::h;
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

    void NewEditAlarmModel::createData()
    {
        auto app = application;
        assert(app != nullptr);

        internalData.push_back(new gui::AlarmTimeItem(
            [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
            [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); }));

        internalData.push_back(new gui::AlarmMusicOptionsItem(
            application,
            utils::translate("app_alarm_clock_sound"),
            soundsPlayer,
            [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text, false); },
            [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); }));

        internalData.push_back(new gui::AlarmSnoozeOptionsItem(utils::translate("app_alarm_clock_snooze")));

        internalData.push_back(new gui::AlarmRRuleOptionsItem(
            application,
            utils::translate("app_alarm_clock_repeat"),
            rRulePresenter,
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

        rRulePresenter->loadRecord(record);
        createData();

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
            alarm->enabled = true;
            alarmsRepository->add(*alarm, [this](bool) { application->returnToPreviousWindow(); });
        }
        list->reset();
        eraseInternalData();
    }
} // namespace app::alarmClock
