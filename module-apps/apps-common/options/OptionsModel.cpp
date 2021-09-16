// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <ApplicationCommon.hpp>
#include "ListView.hpp"
#include "OptionsModel.hpp"

OptionsModel::OptionsModel(app::ApplicationCommon *app) : application(app)
{}

auto OptionsModel::requestRecordsCount() -> unsigned int
{
    return internalData.size();
}

auto OptionsModel::getMinimalItemSpaceRequired() const -> unsigned int
{
    return style::window::label::big_h + gui::option::window::option_bottom_margin;
}

void OptionsModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

auto OptionsModel::getItem(gui::Order order) -> gui::ListItem *
{
    return getRecord(order);
}

void OptionsModel::createData(std::list<gui::Option> &optionList)
{
    for (auto &option : optionList) {
        auto optionItem          = option.build();
        optionItem->deleteByList = false;
        internalData.push_back(optionItem);
    }
}
void OptionsModel::clearData()
{
    eraseInternalData();
}
