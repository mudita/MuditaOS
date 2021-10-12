// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CustomRepeatModel.hpp"

#include <application-alarm-clock/widgets/AlarmClockStyle.hpp>
#include <ListView.hpp>
#include <time/time_locale.hpp>
#include <AppWindow.hpp>

namespace app::alarmClock
{
    CustomRepeatModel::CustomRepeatModel(app::ApplicationCommon *app,
                                         std::shared_ptr<alarmClock::AlarmRRulePresenter> rRulePresenter)
        : application(app), rRulePresenter(rRulePresenter)
    {}

    unsigned int CustomRepeatModel::requestRecordsCount()
    {
        return internalData.size();
    }

    unsigned int CustomRepeatModel::getMinimalItemSpaceRequired() const
    {
        return style::window::label::big_h + style::margins::big;
    }

    void CustomRepeatModel::requestRecords(uint32_t offset, uint32_t limit)
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
        auto app = application;
        for (auto const &[day, selected] : rRulePresenter->getCustomDays()) {
            internalData.push_back(new gui::CustomCheckBoxWithLabel(
                utils::translate(day),
                selected,
                [app](const UTF8 &text) { app->getCurrentWindow()->navBarTemporaryMode(text, false); },
                [app]() { app->getCurrentWindow()->navBarRestoreFromTemporaryMode(); }));
        }

        for (auto &item : internalData) {
            item->deleteByList = false;
        }
    }

    void CustomRepeatModel::loadData()
    {
        list->reset();
        eraseInternalData();

        createData();

        list->rebuildList();
    }

    void CustomRepeatModel::saveCheckedData()
    {
        std::list<utl::Day> days = {};

        for (unsigned int i = 0; i < internalData.size(); i++) {
            if (internalData[i]->isChecked()) {
                days.emplace_back(magic_enum::enum_cast<utl::Day>(i).value());
            }
        }

        rRulePresenter->setOption(AlarmRRulePresenter::RRuleOptions::Custom, days);
    }

    void CustomRepeatModel::eraseData()
    {
        list->reset();
        eraseInternalData();
    }

} // namespace app::alarmClock
