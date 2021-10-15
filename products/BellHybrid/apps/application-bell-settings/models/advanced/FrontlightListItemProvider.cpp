// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrontlightListItemProvider.hpp"
#include "FrontlightModel.hpp"
#include <gui/widgets/ListViewEngine.hpp>
#include <common/widgets/ListItems.hpp>

namespace app::bell_settings
{
    void FrontlightListItemProvider::buildListItems()
    {
        constexpr auto brightnessMin  = 1U;
        constexpr auto brightnessMax  = 10U;
        constexpr auto brightnessStep = 1U;
        constexpr auto itemCount      = 2U;
        internalData.reserve(itemCount);

        auto brightness =
            new gui::NumListItem(model.getBrightnessModel(),
                                 gui::UIntegerSpinner::Range{brightnessMin, brightnessMax, brightnessStep},
                                 utils::translate("app_bell_settings_frontlight_top_message"));
        brightness->onEnter = [this]() { model.setStatus(true); };
        brightness->onExit  = [this]() { model.setStatus(false); };
        brightness->setOnValueChanged([this](const auto val) {
            model.setStatus(true);
            model.setBrightness(val);
        });
        internalData.emplace_back(brightness);

        const auto modeAutoStr      = utils::translate("app_bell_settings_frontlight_mode_auto");
        const auto modeOnDemandsStr = utils::translate("app_bell_settings_frontlight_mode_on_demand");

        auto mode = new gui::UTF8ListItem(model.getModeModel(),
                                          gui::UTF8Spinner::Range{modeOnDemandsStr, modeAutoStr},
                                          utils::translate("app_bell_settings_frontlight_mode_top_message"));
        mode->setOnValueChanged([this, modeAutoStr](const auto &val) {
            model.setMode(val == modeAutoStr ? screen_light_control::ScreenLightMode::Automatic
                                             : screen_light_control::ScreenLightMode::Manual);
        });
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
