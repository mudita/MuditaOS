// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrontlightModel.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <service-evtmgr/screen-light-control/ScreenLightControl.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-evtmgr/ScreenLightControlMessage.hpp>

namespace
{
    constexpr auto minPercent                       = 0.0f;
    constexpr auto maxPercent                       = 100.0f;
    constexpr auto minimumLightOnPercentOffsetValue = 16.0f;
    constexpr auto minBrightness                    = 1U;
    constexpr auto maxBrightness                    = 10U;
    constexpr float multiplier                      = (maxPercent - minimumLightOnPercentOffsetValue) / maxBrightness;

    float fixedValToPercentage(app::bell_settings::AbstractFrontlightModel::Brightness value)
    {
        float scaled = minimumLightOnPercentOffsetValue + (value - minBrightness) * multiplier;
        return std::min(maxPercent, std::max(minPercent, scaled));
    }

    app::bell_settings::AbstractFrontlightModel::Brightness percentageToFixedVal(float percent)
    {
        auto value = (percent - minimumLightOnPercentOffsetValue) / multiplier;
        return std::round(value + minBrightness);
    }
} // namespace

namespace app::bell_settings
{
    FrontlightModel::FrontlightModel(ApplicationCommon *app)
        : app::AsyncCallbackReceiver{app}, brightnessAdapter{std::make_unique<BrightnessAdapter>()},
          modeAdapter{std::make_unique<ModeAdapter>()}, app{app}, autoStr{utils::translate(
                                                                      "app_bell_settings_frontlight_mode_auto")},
          onDemandStr{utils::translate("app_bell_settings_frontlight_mode_on_demand")}
    {
        const auto responseCallback = [this](const auto response) -> bool {
            const auto resp = dynamic_cast<sevm::ScreenLightControlParametersResponse *>(response);
            if (resp) {
                this->brightnessAdapter->update(percentageToFixedVal(resp->getParams().manualModeBrightness));
                this->modeAdapter->update(
                    resp->getMode() == screen_light_control::ScreenLightMode::Automatic ? autoStr : onDemandStr);
                this->onReady();
            }
            return true;
        };
        auto request = AsyncRequest::createFromMessage(std::make_unique<sevm::ScreenLightControlRequestParameters>(),
                                                       service::name::evt_manager);
        request->execute(app, this, responseCallback);
    }
    void FrontlightModel::setStatus(bool onOff)
    {
        app->bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(
                                 onOff ? screen_light_control::Action::turnOn : screen_light_control::Action::turnOff),
                             service::name::evt_manager);
    }
    void FrontlightModel::setMode(screen_light_control::ScreenLightMode mode)
    {
        app->bus.sendUnicast(
            std::make_shared<sevm::ScreenLightControlMessage>(mode == screen_light_control::ScreenLightMode::Automatic
                                                                  ? screen_light_control::Action::enableAutomaticMode
                                                                  : screen_light_control::Action::disableAutomaticMode),
            service::name::evt_manager);
    }
    void FrontlightModel::setBrightness(Brightness value)
    {
        screen_light_control::ConstLinearProgressModeParameters parameters{fixedValToPercentage(value)};
        app->bus.sendUnicast(std::make_shared<sevm::ScreenLightSetConstLinearModeParams>(parameters),
                             service::name::evt_manager);
    }
    gui::AbstractSettingsModel<std::uint8_t> &FrontlightModel::getBrightnessModel()
    {
        return *brightnessAdapter;
    }
    gui::AbstractSettingsModel<UTF8> &FrontlightModel::getModeModel()
    {
        return *modeAdapter;
    }

    void FrontlightModel::restorePreviousState()
    {
        setStatus(false);
        setMode(modeAdapter->getValue() == autoStr ? screen_light_control::ScreenLightMode::Automatic
                                                   : screen_light_control::ScreenLightMode::Manual);
        setBrightness(brightnessAdapter->getValue());
    }
} // namespace app::bell_settings
