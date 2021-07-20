// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimContactsImportModel.hpp"

#include <application-settings/widgets/network/SimContactImportSelectWidget.hpp>
#include <ListView.hpp>
#include <i18n/i18n.hpp>

SimContactsImportModel::SimContactsImportModel(app::Application *app) : application(app)
{
    createData();
}

auto SimContactsImportModel::requestRecordsCount() -> unsigned int
{
    return internalData.size();
}

auto SimContactsImportModel::getMinimalItemSpaceRequired() const -> unsigned int
{
    return style::window::label::big_h + style::margins::big;
}

void SimContactsImportModel::requestRecords(const uint32_t offset, const uint32_t limit)
{
    setupModel(offset, limit);
    list->onProviderDataUpdate();
}

auto SimContactsImportModel::getItem(gui::Order order) -> gui::ListItem *
{
    return getRecord(order);
}

void SimContactsImportModel::createData()
{
    for (auto item : internalData) {
        item->deleteByList = false;
    }
}

void SimContactsImportModel::clearData()
{
    list->reset();
    eraseInternalData();
}
