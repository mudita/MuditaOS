// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>
#include <chrono>
#include <vector>

namespace gui
{
    class ListItemProvider;
}

namespace app
{
    class ApplicationCommon;
}

namespace settings
{
    class Settings;
}
namespace app::bgSounds
{
    class BGSoundsTimerSelectContract
    {
      public:
        using Range = std::vector<std::chrono::minutes>;
        class View
        {
          public:
            ~View() = default;
        };

        class Presenter : public BasePresenter<BGSoundsTimerSelectContract::View>
        {
          public:
            virtual const Range &getTimerValuesRange() const noexcept = 0;
            virtual std::chrono::minutes getCurrentTimerValue() const = 0;
            virtual void setTimerValue(std::chrono::minutes)          = 0;
        };
    };

    class BGSoundsTimerSelectPresenter : public BGSoundsTimerSelectContract::Presenter
    {
        settings::Settings *settings = nullptr;

        const BGSoundsTimerSelectContract::Range &getTimerValuesRange() const noexcept override;
        std::chrono::minutes getCurrentTimerValue() const override;
        void setTimerValue(std::chrono::minutes) override;

      public:
        explicit BGSoundsTimerSelectPresenter(settings::Settings *settings);
    };
} // namespace app::bgSounds
