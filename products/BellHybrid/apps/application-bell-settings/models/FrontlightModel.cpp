// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrontlightModel.hpp"
#include "BellSettingsStyle.hpp"

#include <gui/widgets/ListViewEngine.hpp>
#include <gui/widgets/Style.hpp>
#include <service-time/Constants.hpp>
#include <widgets/BellBaseLayout.hpp>
#include <service-db/Settings.hpp>
#include <service-db/agents/settings/SystemSettings.hpp>

namespace
{
    constexpr auto fmtSpinnerMin   = 1U;
    constexpr auto fmtSpinnerMax   = 10U;
    constexpr auto fmtSpinnerStep  = 1U;
    constexpr auto brightnessLevel = settings::Brightness::brightnessLevel;
} // namespace

namespace app::bell_settings
{
    FrontlightModel::FrontlightModel(app::ApplicationCommon *app,
                                     app::settingsInterface::BellScreenLightSettings *slSettings)
        : application(app), screenLightSettings(slSettings)
    {
        settings.init(service::ServiceProxy{application->weak_from_this()});
    }

    FrontlightModel::~FrontlightModel()
    {}

    auto FrontlightModel::getSpinner() -> gui::UIntegerSpinner *
    {
        if (frontlightSetSpinner != nullptr) {
            return frontlightSetSpinner;
        }
        else {
            LOG_ERROR("Spinner not initialized !");
            return nullptr;
        }
    }

    void FrontlightModel::createData()
    {
        frontlightSetSpinner = new gui::UIntegerSpinner(
            gui::UIntegerSpinner::Range{fmtSpinnerMin, fmtSpinnerMax, fmtSpinnerStep}, gui::Boundaries::Fixed);
        frontlightSetSpinner->setMaximumSize(style::bell_base_layout::w, style::bell_base_layout::h);
        frontlightSetSpinner->setFont(gui::bell_settings_style::time_fmt_set_list_item::font);

        frontlightSetSpinner->setAlignment(
            gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        frontlightSetSpinner->setEdges(gui::RectangleEdge::None);
        frontlightSetSpinner->setCurrentValue(fmtSpinnerMin);
        frontlightSetSpinner->setFocusEdges(gui::RectangleEdge::None);
    }

    void FrontlightModel::saveData()
    {
        const int value = frontlightSetSpinner->getCurrentValue() * 10;
        LOG_DEBUG("Storing frontlight value %i", value);
        screenLightSettings->setBrightness(static_cast<float>(value));
        settings.setValue(
            brightnessLevel, std::to_string(static_cast<std::int32_t>(value)), settings::SettingsScope::Global);
    }

    void FrontlightModel::loadData()
    {
        bsp::eink_frontlight::BrightnessPercentage brightnessValue =
            utils::getNumericValue<float>(settings.getValue(brightnessLevel, settings::SettingsScope::Global));
        LOG_DEBUG("Reading frontlight value %0.1f", brightnessValue);
        // prevent showing spinner value less than minimum when database empty
        if (static_cast<unsigned int>(brightnessValue) < fmtSpinnerMin) {
            brightnessValue = static_cast<bsp::eink_frontlight::BrightnessPercentage>(fmtSpinnerMin);
        }
        frontlightSetSpinner->setCurrentValue(static_cast<int>(brightnessValue / 10));
    }

    void FrontlightModel::setLiveBrightness(const int value)
    {
        screenLightSettings->setStatus(true);
        if ((value < static_cast<int>(fmtSpinnerMin)) || (value > static_cast<int>(fmtSpinnerMax))) {
            return;
        }
        screenLightSettings->setBrightness(static_cast<float>(value * 10));
    }
} // namespace app::bell_settings
