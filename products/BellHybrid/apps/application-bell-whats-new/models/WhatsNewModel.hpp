// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

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

namespace app::whatsNew::models
{
    struct Feature
    {
        const std::string description;
        const std::string iconName;
    };

    class WhatsNewModel
    {
      public:
        explicit WhatsNewModel(app::ApplicationCommon *app, settings::Settings *settings);

      private:
        app::ApplicationCommon *app{nullptr};
        settings::Settings *settings{nullptr};
        std::vector<Feature> features;
    };
} // namespace app::whatsNew::models
