// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <InternalModel.hpp>
#include <ListItemProvider.hpp>
#include <widgets/DateOrTimeListItem.hpp>

namespace gui
{
    class SettingsTimeItem;
};

class FromTimeToTimeModel : public app::InternalModel<gui::DateOrTimeListItem *>, public gui::ListItemProvider
{
  public:
    FromTimeToTimeModel(app::Application *application);

    void loadData(std::shared_ptr<utils::time::FromTillDate> fromTillDate);
    void saveData(std::shared_ptr<utils::time::FromTillDate> fromTillDate);

    gui::ListItem *getItem(gui::Order order) override;
    [[nodiscard]] unsigned int getMinimalItemHeight() const override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;
    [[nodiscard]] unsigned int requestRecordsCount() override;

  private:
    app::Application *app               = nullptr;
    gui::SettingsTimeItem *fromTimeItem = nullptr;
    gui::SettingsTimeItem *toTimeItem   = nullptr;

    void createData();
};
