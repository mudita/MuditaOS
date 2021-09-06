// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-alarm-clock/models/AlarmsRepository.hpp"
#include "application-alarm-clock/data/AlarmsData.hpp"
#include <application-alarm-clock/widgets/CustomCheckBoxWithLabel.hpp>
#include <application-alarm-clock/presenter/AlarmRRulePresenter.hpp>

#include <Application.hpp>
#include <InternalModel.hpp>
#include <ListItemProvider.hpp>

namespace app::alarmClock
{
    class CustomRepeatListItemProvider : public InternalModel<gui::CustomCheckBoxWithLabel *>,
                                         public gui::ListItemProvider
    {
      public:
        CustomRepeatListItemProvider() = default;

        virtual void loadData()        = 0;
        virtual void eraseData()       = 0;
        virtual void saveCheckedData() = 0;
    };

    class CustomRepeatModel : public CustomRepeatListItemProvider
    {
        app::ApplicationCommon *application = nullptr;
        std::shared_ptr<app::alarmClock::AlarmRRulePresenter> rRulePresenter;
        void createData();

      public:
        CustomRepeatModel(app::ApplicationCommon *app, std::shared_ptr<alarmClock::AlarmRRulePresenter> rRulePresenter);

        [[nodiscard]] unsigned int getMinimalItemSpaceRequired() const override;
        [[nodiscard]] unsigned int requestRecordsCount() override;
        [[nodiscard]] gui::ListItem *getItem(gui::Order order) override;
        void requestRecords(uint32_t offset, uint32_t limit) override;

        void loadData() override;
        void saveCheckedData() override;
        void eraseData() override;
    };
} // namespace app::alarmClock
