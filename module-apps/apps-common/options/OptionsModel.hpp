// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Option.hpp"
#include "InternalModel.hpp"
#include <ListItemProvider.hpp>
#include <Application.hpp>

class OptionsModel : public app::InternalModel<gui::ListItem *>, public gui::ListItemProvider
{
  private:
    app::ApplicationCommon *application   = nullptr;
    unsigned int minimalItemSpaceRequired = 0;

  public:
    explicit OptionsModel(app::ApplicationCommon *app,
                          unsigned int minimalItemSpaceRequired = style::window::label::big_h +
                                                                  gui::option::window::option_bottom_margin);

    void clearData();
    void createData(std::list<gui::Option> &optionList);

    [[nodiscard]] auto requestRecordsCount() -> unsigned int override;
    [[nodiscard]] auto getMinimalItemSpaceRequired() const -> unsigned int override;

    auto getItem(gui::Order order) -> gui::ListItem * override;

    void requestRecords(uint32_t offset, uint32_t limit) override;
};
