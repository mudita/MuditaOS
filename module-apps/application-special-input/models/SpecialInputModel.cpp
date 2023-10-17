// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SpecialInputModel.hpp"
#include "SpecialInputTableWidget.hpp"
#include <ListView.hpp>

SpecialInputModel::SpecialInputModel(app::ApplicationCommon *app) : application(app)
{}

auto SpecialInputModel::requestRecordsCount() -> unsigned int
{
    return internalData.size();
}

auto SpecialInputModel::getMinimalItemSpaceRequired() const -> unsigned int
{
    return specialInputStyle::specialCharacterTableWidget::window_grid_h;
}

void SpecialInputModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

auto SpecialInputModel::getItem(gui::Order order) -> gui::ListItem *
{
    return getRecord(order);
}

auto SpecialInputModel::getMaxGridElements() const -> ssize_t
{
    return (specialInputStyle::specialCharacterTableWidget::window_grid_w /
            specialInputStyle::specialCharacterTableWidget::char_grid_w) *
           (specialInputStyle::specialCharacterTableWidget::window_grid_h /
            specialInputStyle::specialCharacterTableWidget::char_grid_h);
}

void SpecialInputModel::buildGrid(const std::vector<char32_t> &elements)
{
    auto add_start = elements.begin();
    while (add_start != elements.end()) {
        auto carrier_size = std::min(std::distance(add_start, elements.end()), getMaxGridElements());
        std::list<gui::Carrier> carriers;
        for (auto val : std::list(add_start, add_start + carrier_size)) {
            if (val == specialInputStyle::symbol_for_newline) {
                carriers.push_back(gui::generateNewLineSign());
            }
            else {
                carriers.push_back(gui::generateCharSign(val));
            }
        }
        add_start = add_start + carrier_size;
        internalData.push_back(new gui::SpecialInputTableWidget(application, std::move(carriers)));
    }
}

void SpecialInputModel::createData(specialInputStyle::CharactersType type)
{
    auto create = [&](const auto &vector) {
        buildGrid(vector);
        for (auto &item : internalData) {
            item->deleteByList = false;
        }
        list->rebuildList();
    };

    switch (type) {
    case specialInputStyle::CharactersType::SpecialCharacters:
        create(std::vector<char32_t>{specialInputStyle::special_chars.begin(), specialInputStyle::special_chars.end()});
        break;
    case specialInputStyle::CharactersType::Emoji:
        create(std::vector<char32_t>{specialInputStyle::emojis.begin(), specialInputStyle::emojis.end()});
        break;
    }
}

void SpecialInputModel::clearData()
{
    list->reset();
    eraseInternalData();
}
