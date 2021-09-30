// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "AlarmsRepository.hpp"
#include "Application.hpp"
#include <ListItemProvider.hpp>
#include <DatabaseModel.hpp>
#include <module-db/Interface/AlarmEventRecord.hpp>
#include <module-db/Common/Query.hpp>

namespace app::alarmClock
{
    class AlarmsListItemProvider : public app::DatabaseModel<AlarmEventRecord>, public gui::ListItemProvider
    {
      public:
        explicit AlarmsListItemProvider(app::ApplicationCommon *app);
    };

    class AlarmsModel : public AlarmsListItemProvider
    {
      public:
        AlarmsModel(app::ApplicationCommon *app, std::shared_ptr<AbstractAlarmsRepository> alarmsRepository);

        void requestRecords(uint32_t offset, uint32_t limit) override;
        bool updateRecords(std::vector<AlarmEventRecord> records) override;

        // virtual methods for ListViewProvider
        [[nodiscard]] unsigned int getMinimalItemSpaceRequired() const override;
        [[nodiscard]] gui::ListItem *getItem(gui::Order order) override;
        [[nodiscard]] unsigned int requestRecordsCount() override;

      private:
        bool onAlarmsRetrieved(const std::vector<AlarmEventRecord> &records, unsigned int alarmsRepoCount);
        void onAlarmEnableChange(const std::shared_ptr<AlarmEventRecord> record);
        std::shared_ptr<AbstractAlarmsRepository> alarmsRepository;
    };
} // namespace app::alarmClock
