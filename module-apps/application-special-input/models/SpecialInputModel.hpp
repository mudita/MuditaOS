// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "SpecialCharactersTableStyle.hpp"
#include <InternalModel.hpp>
#include <Application.hpp>

#include <ListItemProvider.hpp>

class SpecialInputModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
{
  private:
    app::ApplicationCommon *application = nullptr;
    [[nodiscard]] auto getMaxGridElements() const -> ssize_t;
    void buildGrid(const std::vector<char32_t> &elements);

  public:
    SpecialInputModel(app::ApplicationCommon *app);
    void clearData();
    void createData(specialInputStyle::CharactersType type = specialInputStyle::CharactersType::SpecialCharacters);

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;

    [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;

    auto getItem(gui::Order order) -> gui::ListItem * override;

    void requestRecords(const uint32_t offset, const uint32_t limit) override;
};
