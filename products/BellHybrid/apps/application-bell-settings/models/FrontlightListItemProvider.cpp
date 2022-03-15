// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrontlightListItemProvider.hpp"
#include <common/models/FrontlightModel.hpp>
#include <gui/widgets/ListViewEngine.hpp>
#include <common/widgets/list_items/Text.hpp>
#include <common/widgets/list_items/Numeric.hpp>

namespace app::bell_settings
{
    void FrontlightListItemProvider::buildListItems()
    {
        constexpr auto brightnessMin  = 1U;
        constexpr auto brightnessMax  = 10U;
        constexpr auto brightnessStep = 1U;
        constexpr auto itemCount      = 2U;
        internalData.reserve(itemCount);

        auto brightness = new list_items::Numeric(
            list_items::Numeric::spinner_type::range{brightnessMin, brightnessMax, brightnessStep},
            model.getBrightnessModel(),
            utils::translate("app_bell_settings_frontlight_top_message"));
        brightness->set_on_value_change_cb([this](const auto val) {
            model.setStatus(true);
            model.setBrightness(val);
        });
        internalData.emplace_back(brightness);

        const auto modeAutoStr      = utils::translate("app_bell_settings_frontlight_mode_auto");
        const auto modeOnDemandsStr = utils::translate("app_bell_settings_frontlight_mode_on_demand");

        auto mode    = new list_items::Text(list_items::Text::spinner_type ::range{modeOnDemandsStr, modeAutoStr},
                                         model.getModeModel(),
                                         utils::translate("app_bell_settings_frontlight_mode_top_message"));
        mode->onExit = [this, mode, modeAutoStr]() {
            model.setMode(mode->value() == modeAutoStr ? screen_light_control::ScreenLightMode::Automatic
                                                       : screen_light_control::ScreenLightMode::Manual);
        };
        internalData.emplace_back(mode);
    }
    FrontlightListItemProvider::FrontlightListItemProvider(AbstractFrontlightModel &model) : model{model}
    {
        buildListItems();
        for (auto item : internalData) {
            item->deleteByList = false;
        }
    }
} // namespace app::bell_settings
