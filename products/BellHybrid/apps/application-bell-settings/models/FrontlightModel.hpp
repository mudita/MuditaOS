// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/InternalModel.hpp>
#include <ApplicationBellSettings.hpp>
#include <widgets/Spinner.hpp>
#include <service-db/Settings.hpp>

class ApplicationCommon;

namespace app::bell_settings
{
    class FrontlightModel
    {
      public:
        explicit FrontlightModel(app::ApplicationCommon *app,
                                 app::settingsInterface::BellScreenLightSettings *slSettings);
        ~FrontlightModel();

        auto saveData() -> void;
        auto loadData() -> void;
        auto createData() -> void;

        [[nodiscard]] auto getSpinner() -> gui::Spinner *;

      private:
        app::ApplicationCommon *application                                  = nullptr;
        app::settingsInterface::BellScreenLightSettings *screenLightSettings = nullptr;
        gui::Spinner *frontlightSetSpinner                                   = nullptr;
        mutable settings::Settings settings;
    };
} // namespace app::bell_settings
