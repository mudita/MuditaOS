// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "RelaxationListItemProvider.hpp"
#include "common/widgets/ListItems.hpp"
#include "data/BellSettingsStyle.hpp"

#include <gui/widgets/ListViewEngine.hpp>

namespace app::bell_settings
{
    using namespace gui;

    RelaxationListItemProvider::RelaxationListItemProvider(std::unique_ptr<AbstractRelaxationFadeModel> &&model)
        : model{std::move(model)}
    {
        buildListItems();
    }

    void RelaxationListItemProvider::buildListItems()
    {
        constexpr auto itemCount = 1U;
        internalData.reserve(itemCount);

        auto onOff = new OnOffListItem(model->getFade(), utils::translate("app_bell_settings_relaxation_fade_message"));
        onOff->onProceed = [onOff, this]() {
            if (not onOff->isActive()) {
                this->onExit();
                return true;
            }
            return false;
        };
        internalData.emplace_back(onOff);

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

    auto RelaxationListItemProvider::requestRecords(std::uint32_t offset, std::uint32_t limit) -> void
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    auto RelaxationListItemProvider::getItem(Order order) -> ListItem *
    {
        return getRecord(order);
    }

    auto RelaxationListItemProvider::requestRecordsCount() -> unsigned int
    {
        return internalData.size();
    }

    auto RelaxationListItemProvider::getMinimalItemSpaceRequired() const -> unsigned int
    {
        return style::sidelistview::list_item::w;
    }

    auto RelaxationListItemProvider::getListItems() -> std::vector<BellSideListItemWithCallbacks *>
    {
        return internalData;
    }
} // namespace app::bell_settings
