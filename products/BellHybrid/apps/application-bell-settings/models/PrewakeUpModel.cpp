// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PrewakeUpModel.hpp"

#include <gui/widgets/ListViewEngine.hpp>
#include <gui/widgets/Style.hpp>
#include <gui/widgets/Text.hpp>

namespace app::bell_settings
{
    PrewakeUpModel::PrewakeUpModel(app::Application *app) : application(app)
    {}

    PrewakeUpModel::~PrewakeUpModel()
    {
        clearData();
    }

    auto PrewakeUpModel::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto PrewakeUpModel::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::sidelistview::list_item::w;
    }

    auto PrewakeUpModel::getItem(gui::Order order) -> gui::ListItem *
    {
        return getRecord(order);
    }

    void PrewakeUpModel::createData()
    {
        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

    void PrewakeUpModel::clearData()
    {
        eraseInternalData();
    }

    void PrewakeUpModel::saveData()
    {
        // TODO
    }

    void PrewakeUpModel::loadData()
    {
        // TODO
    }

    auto PrewakeUpModel::requestRecords(uint32_t offset, uint32_t limit) -> void
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }
} // namespace app::bell_settings
