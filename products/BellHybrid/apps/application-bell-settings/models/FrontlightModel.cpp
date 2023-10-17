// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <common/models/FrontlightModel.hpp>

#include <apps-common/ApplicationCommon.hpp>
#include <service-evtmgr/screen-light-control/ScreenLightControl.hpp>
#include <service-evtmgr/ServiceEventManagerName.hpp>
#include <service-evtmgr/ScreenLightControlMessage.hpp>

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
                const auto brightness = frontlight_utils::percentageToFixedVal(resp->getParams().manualModeBrightness);
                this->brightnessAdapter->update(brightness);
                this->modeAdapter->update(
                    resp->getMode() == screen_light_control::ScreenLightMode::Automatic ? autoStr : onDemandStr);
                if (this->onReady) {
                    this->onReady();
                    setBacklight(BacklightState::On);
                    setBrightness(brightness);
                }
            }
            return true;
        };
        auto request = AsyncRequest::createFromMessage(std::make_unique<sevm::ScreenLightControlRequestParameters>(),
                                                       service::name::evt_manager);
        request->execute(app, this, responseCallback);
    }
    void FrontlightModel::setBacklight(BacklightState state)
    {
        app->bus.sendUnicast(std::make_shared<sevm::ScreenLightControlMessage>(
                                 state == BacklightState::On ? screen_light_control::Action::turnOn
                                                             : screen_light_control::Action::turnOff),
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
    void FrontlightModel::setBrightness(frontlight_utils::Brightness value)
    {
        screen_light_control::ConstLinearProgressModeParameters parameters{
            frontlight_utils::fixedValToPercentage(value)};
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

    void FrontlightModel::revertConfig()
    {
        setMode(modeAdapter->getValue() == autoStr ? screen_light_control::ScreenLightMode::Automatic
                                                   : screen_light_control::ScreenLightMode::Manual);
        setBrightness(brightnessAdapter->getValue());
    }

    void FrontlightModel::saveConfig()
    {
        configSaved = true;
    }

    bool FrontlightModel::isConfigSaved()
    {
        return configSaved;
    }
} // namespace app::bell_settings
