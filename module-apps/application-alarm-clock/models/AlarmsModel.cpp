// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "AlarmsModel.hpp"
#include "application-alarm-clock/widgets/AlarmItem.hpp"
#include "application-alarm-clock/widgets/AlarmClockStyle.hpp"
#include "application-alarm-clock/windows/AlarmClockOptions.hpp"
#include "messages/OptionsWindow.hpp"
#include <ListView.hpp>
#include <InputEvent.hpp>

namespace app::alarmClock
{
    AlarmsListItemProvider::AlarmsListItemProvider(ApplicationCommon *app) : DatabaseModel(app)
    {}

    AlarmsModel::AlarmsModel(app::ApplicationCommon *app, std::shared_ptr<AbstractAlarmsRepository> alarmsRepository)
        : AlarmsListItemProvider(app), alarmsRepository{std::move(alarmsRepository)}
    {}

    unsigned int AlarmsModel::requestRecordsCount()
    {
        return recordsCount;
    }

    void AlarmsModel::requestRecords(uint32_t offset, uint32_t limit)
    {
        alarmsRepository->getLimited(
            offset, limit, [this](const std::vector<AlarmEventRecord> &records, unsigned int alarmsRepoCount) {
                return onAlarmsRetrieved(records, alarmsRepoCount);
            });
    }

    unsigned int AlarmsModel::getMinimalItemSpaceRequired() const
    {
        return style::alarmClock::window::item::height;
    }

    gui::ListItem *AlarmsModel::getItem(gui::Order order)
    {
        auto record = getRecord(order);
        if (record == nullptr) {
            LOG_DEBUG("Empty record in AlarmsModel::GetItem");
            return nullptr;
        }

        auto item               = new gui::AlarmItem(AlarmPresenter(record));
        item->activatedCallback = [this, record](gui::Item &) {
            record->enabled = !record->enabled;
            alarmsRepository->update(*record, nullptr);
            return true;
        };
        item->inputCallback = [this, record = record.get()](gui::Item &, const gui::InputEvent &event) {
            if (event.isShortRelease(gui::KeyCode::KEY_LF)) {
                application->switchWindow(utils::translate("app_alarm_clock_options_title"),
                                          std::make_unique<gui::OptionsWindowOptions>(
                                              alarmsListOptions(application, *record, *alarmsRepository)));
            }
            return false;
        };
        return item;
    }

    bool AlarmsModel::updateRecords(std::vector<AlarmEventRecord> records)
    {
        DatabaseModel::updateRecords(std::move(records));
        list->onProviderDataUpdate();
        return true;
    }

    bool AlarmsModel::onAlarmsRetrieved(const std::vector<AlarmEventRecord> &records, unsigned int alarmsRepoCount)
    {
        if (recordsCount != alarmsRepoCount) {
            recordsCount = alarmsRepoCount;
            list->reSendLastRebuildRequest();
            return false;
        }

        return updateRecords(records);
    }
} // namespace app::alarmClock
