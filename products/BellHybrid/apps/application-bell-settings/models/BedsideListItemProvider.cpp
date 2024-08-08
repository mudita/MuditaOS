// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BedsideListItemProvider.hpp"
#include <common/models/BedsideModel.hpp>
#include <common/models/FrontlightModel.hpp>
#include <gui/widgets/ListViewEngine.hpp>
#include <common/widgets/list_items/Numeric.hpp>
#include <LanguageUtils.hpp>
#include <i18n/i18n.hpp>

namespace app::bell_settings
{
    using namespace gui;

    BedsideListItemProvider::BedsideListItemProvider(std::unique_ptr<AbstractBedsideModel> &&bedsideModel,
                                                     std::unique_ptr<AbstractFrontlightModel> &&frontlightModel)
        : bedsideModel{std::move(bedsideModel)}, frontlightModel{std::move(frontlightModel)}
    {
        buildListItems();
    }

    auto BedsideListItemProvider::getListItems() -> std::vector<BellSideListItemWithCallbacks *>
    {
        return internalData;
    }

    auto BedsideListItemProvider::requestRecords(std::uint32_t offset, std::uint32_t limit) -> void
    {
        setupModel(offset, limit);
        list->onProviderDataUpdate();
    }

    auto BedsideListItemProvider::getItem(Order order) -> ListItem *
    {
        return getRecord(order);
    }

    auto BedsideListItemProvider::requestRecordsCount() -> unsigned
    {
        return internalData.size();
    }

    auto BedsideListItemProvider::getMinimalItemSpaceRequired() const -> unsigned
    {
        return style::sidelistview::list_item::w;
    }

    auto BedsideListItemProvider::buildListItems() -> void
    {
        constexpr auto itemCount = 2U;
        internalData.reserve(itemCount);

        constexpr auto bedsideTimeStep = 1U;
        constexpr auto bedsideTimeMin  = 1U;
        constexpr auto bedsideTimeMax  = 60U;
        auto bedsideTimeSpinner        = new list_items::Numeric(
            list_items::Numeric::spinner_type::range{bedsideTimeMin, bedsideTimeMax, bedsideTimeStep},
            bedsideModel->getTimeModel(),
            utils::translate("app_bell_settings_bedside_lamp_time"),
            utils::language::getCorrectMinutesNumeralForm(bedsideModel->getTimeModel().getValue()));
        bedsideTimeSpinner->set_on_value_change_cb([bedsideTimeSpinner](const auto &val) {
            bedsideTimeSpinner->setBottomDescriptionText(utils::language::getCorrectMinutesNumeralForm(val));
        });
        internalData.emplace_back(bedsideTimeSpinner);

        constexpr auto bedsideBrightnessStep = 1U;
        constexpr auto bedsideBrightnessMin  = 1U;
        constexpr auto bedsideBrightnessMax  = 10U;
        auto bedsideBrightnessSpinner        = new list_items::Numeric(
            list_items::Numeric::spinner_type::range{bedsideBrightnessMin, bedsideBrightnessMax, bedsideBrightnessStep},
            bedsideModel->getBrightnessModel(),
            utils::translate("app_bell_settings_bedside_lamp_intensity"));
        bedsideBrightnessSpinner->set_on_value_change_cb([this](const auto &val) {
            this->frontlightModel->setBacklight(BacklightState::On);
            this->frontlightModel->setBrightness(val);
        });
        bedsideBrightnessSpinner->onEnter = [this, bedsideBrightnessSpinner]() {
            this->frontlightModel->setBacklight(BacklightState::On);
            this->frontlightModel->setBrightness(bedsideBrightnessSpinner->value());
        };
        bedsideBrightnessSpinner->onExit = [this]() {
            this->frontlightModel->revertConfig();
            this->frontlightModel->setBacklight(BacklightState::On);
        };
        internalData.emplace_back(bedsideBrightnessSpinner);

        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }

    auto BedsideListItemProvider::clearData() -> void
    {
        list->reset();
        eraseInternalData();
    }
} // namespace app::bell_settings
