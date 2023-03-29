// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/AsyncTask.hpp>
#include <common/models/SettingsModel.hpp>
#include <common/data/FrontlightUtils.hpp>
#include <screen-light-control/ScreenLightControlParameters.hpp>

namespace app
{
    class ApplicationCommon;
}

namespace app::bell_settings
{
    enum class BacklightState
    {
        On,
        Off
    };

    class AbstractFrontlightModel
    {
      public:
        virtual ~AbstractFrontlightModel()                               = default;
        virtual void setBrightness(frontlight_utils::Brightness value)   = 0;
        virtual void setMode(screen_light_control::ScreenLightMode mode) = 0;
        virtual void setBacklight(BacklightState state)                  = 0;
        virtual void revertConfig()                                      = 0;
        virtual void saveConfig()                                        = 0;
        virtual bool isConfigSaved()                                     = 0;

        virtual gui::AbstractSettingsModel<std::uint8_t> &getBrightnessModel() = 0;
        virtual gui::AbstractSettingsModel<UTF8> &getModeModel()               = 0;

        std::function<void()> onReady;
    };

    class FrontlightModel : public AbstractFrontlightModel, public AsyncCallbackReceiver
    {
      public:
        explicit FrontlightModel(ApplicationCommon *app);

        gui::AbstractSettingsModel<std::uint8_t> &getBrightnessModel() override;
        gui::AbstractSettingsModel<UTF8> &getModeModel() override;

        void setBrightness(frontlight_utils::Brightness value) override;
        void setMode(screen_light_control::ScreenLightMode mode) override;
        void setBacklight(BacklightState state) override;
        void revertConfig() override;
        void saveConfig() override;
        bool isConfigSaved() override;

      private:
        template <typename ValueT>
        class Adapter : public gui::AbstractSettingsModel<ValueT>
        {
          public:
            void update(ValueT val)
            {
                currentValue = val;
            }
            void setValue(ValueT) override
            {}
            ValueT getValue() const override
            {
                return currentValue;
            }

          private:
            ValueT currentValue{};
        };
        using BrightnessAdapter = Adapter<std::uint8_t>;
        using ModeAdapter       = Adapter<UTF8>;

        std::unique_ptr<BrightnessAdapter> brightnessAdapter;
        std::unique_ptr<ModeAdapter> modeAdapter;
        ApplicationCommon *app{};
        bool configSaved{false};
        const std::string autoStr;
        const std::string onDemandStr;
    };

} // namespace app::bell_settings
