// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-settings-new/data/SettingsItemData.hpp"
#include "application-settings-new/widgets/ApnListItem.hpp"
#include "application-settings-new/widgets/SettingsStyle.hpp"
#include "InternalModel.hpp"
#include "Application.hpp"

#include <ListItemProvider.hpp>

class NewApnModel : public app::InternalModel<gui::ApnListItem *>, public gui::ListItemProvider
{
    app::Application *application = nullptr;

  public:
    NewApnModel(app::Application *app);

    void clearData();
    void saveData(std::shared_ptr<packet_data::APN::Config> apnRecord);
    void loadData(std::shared_ptr<packet_data::APN::Config> apnRecord);

    void createData();

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;

    [[nodiscard]] auto getMinimalItemHeight() const -> unsigned int override;

    auto getItem(gui::Order order) -> gui::ListItem * override;

    void requestRecords(const uint32_t offset, const uint32_t limit) override;
    void apnDataChanged();
};
