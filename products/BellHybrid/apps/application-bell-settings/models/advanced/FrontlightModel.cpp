// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "FrontlightModel.hpp"

#include <apps-common/ApplicationCommon.hpp>
#include <service-evtmgr/screen-light-control/ScreenLightControl.hpp>
#include <service-evtmgr/Constants.hpp>
#include <service-evtmgr/ScreenLightControlMessage.hpp>

namespace app::bell_settings
{
    FrontlightModel::FrontlightModel(ApplicationCommon *app)
        : app::AsyncCallbackReceiver{app}, brightnessAdapter{std::make_unique<BrightnessAdapter>()},
          modeAdapter{std::make_unique<ModeAdapter>()}, app{app}, autoStr{utils::translate("app_bell_fl_mode_auto")},
          onDemandStr{utils::translate("app_bell_fl_mode_on_demand")}
    {
        const auto responseCallback = [this](const auto response) -> bool {
            constexpr auto brightnessMultiplier = 10;
            const auto resp                     = dynamic_cast<sevm::ScreenLightControlParametersResponse *>(response);
            if (resp) {
                this->brightnessAdapter->update(resp->getParams().manualModeBrightness / brightnessMultiplier);
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
    void FrontlightModel::setBrightness(float value)
    {
        screen_light_control::ManualModeParameters parameters{value};
        app->bus.sendUnicast(std::make_shared<sevm::ScreenLightSetManualModeParams>(parameters),
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
} // namespace app::bell_settings
