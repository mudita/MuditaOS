// Copyright (c) 2017-2025, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "WhatsNewFeaturesModel.hpp"
#include <apps-common/BasePresenter.hpp>

namespace settings
{
    class Settings;
}

namespace app
{
    class ApplicationCommon;
    class AbstractBatteryModel;
    class AbstractLowBatteryInfoModel;
} // namespace app

namespace app::whatsnew
{
    class WhatsNewMainContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public BasePresenter<WhatsNewMainContract::View>
        {
          public:
            virtual ~Presenter()                       = default;
            virtual auto setCurrentOsVersion() -> void = 0;
            virtual auto getFeaturesCount() -> std::size_t = 0;
            virtual auto showFeatures() -> void        = 0;
        };
    };

    class WhatsNewMainPresenter : public WhatsNewMainContract::Presenter
    {
      public:
        WhatsNewMainPresenter(app::ApplicationCommon *app,
                              models::WhatsNewFeaturesModel &model,
                              AbstractBatteryModel &batteryModel,
                              AbstractLowBatteryInfoModel &lowBatteryInfoModel,
                              settings::Settings *settings);
        auto setCurrentOsVersion() -> void override;
        auto getFeaturesCount() -> std::size_t override;
        auto showFeatures() -> void override;

      private:
        app::ApplicationCommon *app;
        models::WhatsNewFeaturesModel &model;
        AbstractBatteryModel &batteryModel;
        AbstractLowBatteryInfoModel &lowBatteryInfoModel;
        settings::Settings *settings{nullptr};
    };
} // namespace app::whatsnew
