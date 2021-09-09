// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "FactoryData.hpp"
#include "TechnicalInformationRepository.hpp"

#include <Application.hpp>
#include <InternalModel.hpp>
#include <ListItemProvider.hpp>

class TechnicalInformationModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
{
  private:
    app::ApplicationCommon *application = nullptr;
    std::unique_ptr<AbstractFactoryData> factoryData;
    std::shared_ptr<AbstractTechnicalInformationRepository> technicalInformationRepository = nullptr;

  public:
    explicit TechnicalInformationModel(std::unique_ptr<AbstractFactoryData> &&factoryData,
                                       std::unique_ptr<AbstractTechnicalInformationRepository> repository);

    void createData();
    void clearData();

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
    [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;
    auto getItem(gui::Order order) -> gui::ListItem * override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;
    void requestImei(std::function<void()> onImeiReadCallback);
};
