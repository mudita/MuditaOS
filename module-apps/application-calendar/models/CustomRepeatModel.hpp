// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include "application-calendar/widgets/CalendarStyle.hpp"
#include "application-calendar/widgets/CalendarListItem.hpp"
#include "module-apps/application-calendar/data/CalendarData.hpp"
#include "InternalModel.hpp"
#include <ListItemProvider.hpp>

class CustomRepeatModel : public app::InternalModel<gui::CalendarListItem *>, public gui::ListItemProvider
{
    app::Application *application = nullptr;

  public:
    CustomRepeatModel(app::Application *app);

    void loadData(const std::shared_ptr<WeekDaysRepeatData> &data);

    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    [[nodiscard]] unsigned int requestRecordsCount() override;
    gui::ListItem *getItem(gui::Order order) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;
    std::vector<bool> getIsCheckedData();

  private:
    void createData(const std::shared_ptr<WeekDaysRepeatData> &data);
};
