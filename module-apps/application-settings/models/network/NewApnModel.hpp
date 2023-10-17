// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-settings/widgets/network/ApnListItem.hpp>

#include <Application.hpp>
#include <InternalModel.hpp>

class NewApnModel : public app::InternalModel<gui::ApnListItem *>, public gui::ListItemProvider
{
    app::ApplicationCommon *application = nullptr;

  public:
    explicit NewApnModel(app::ApplicationCommon *app);

    void clearData();
    void saveData(const std::shared_ptr<packet_data::APN::Config> &apnRecord);
    void loadData(const std::shared_ptr<packet_data::APN::Config> &apnRecord);

    void createData();

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;

    [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;

    auto getItem(gui::Order order) -> gui::ListItem * override;

    void requestRecords(uint32_t offset, uint32_t limit) override;
    void apnNameChanged(const std::string &newName);
};
