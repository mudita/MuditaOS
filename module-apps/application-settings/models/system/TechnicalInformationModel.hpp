// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FactoryData.hpp"

#include <InternalModel.hpp>
#include <ListItemProvider.hpp>

class TechnicalInformationModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
{
  private:
    static constexpr auto imei = "AA-BBBBBB-CCCCC-D";

    std::unique_ptr<AbstractFactoryData> factoryData;

  public:
    explicit TechnicalInformationModel(std::unique_ptr<AbstractFactoryData> &&factoryData);

    void createData();

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
    [[nodiscard]] auto getMinimalItemHeight() const -> unsigned int override;
    auto getItem(gui::Order order) -> gui::ListItem * override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;
};
