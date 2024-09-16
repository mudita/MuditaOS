// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <string>
#include <vector>

namespace app
{
    class ApplicationCommon;
}

namespace settings
{
    class Settings;
}

namespace app::whatsnew::models
{
    struct Feature
    {
        std::string title;
        std::string description;
        std::string iconName;
    };

    class WhatsNewFeaturesModel
    {
      public:
        WhatsNewFeaturesModel(app::ApplicationCommon *app, settings::Settings *settings);

        auto getFeatures() -> std::vector<Feature>;
        auto setCurrentOsVersion() -> void;

      private:
        app::ApplicationCommon *app{nullptr};
        settings::Settings *settings{nullptr};
        std::vector<Feature> features;
    };
} // namespace app::whatsnew::models
