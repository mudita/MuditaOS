// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"
#include "InternalModel.hpp"
#include <ListItemProvider.hpp>
#include "widgets/DateWidget.hpp"
#include "widgets/TimeWidget.hpp"
#include "module-db/Interface/EventsRecord.hpp"
#include "module-apps/widgets/DateOrTimeListItem.hpp"

class DateAndTimeModel : public app::InternalModel<gui::DateOrTimeListItem *>, public gui::ListItemProvider
{
    app::Application *application = nullptr;
    gui::DateWidget *dateItem     = nullptr;
    gui::TimeWidget *timeItem     = nullptr;

  public:
    DateAndTimeModel(app::Application *app);

    void loadData(std::shared_ptr<EventsRecord> record);
    void saveData(std::shared_ptr<EventsRecord> record);

    gui::ListItem *getItem(gui::Order order) override;
    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;
    [[nodiscard]] unsigned int requestRecordsCount() override;

  private:
    void createData();
};
