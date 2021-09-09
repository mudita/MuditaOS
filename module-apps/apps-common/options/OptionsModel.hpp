// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Option.hpp"
#include "InternalModel.hpp"
#include <ListItemProvider.hpp>

class OptionsModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
{
    app::ApplicationCommon *application = nullptr;

  public:
    explicit OptionsModel(app::ApplicationCommon *app);

    void clearData();
    void createData(std::list<gui::Option> &optionList);

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
    [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;

    auto getItem(gui::Order order) -> gui::ListItem * override;

    void requestRecords(uint32_t offset, uint32_t limit) override;
};
