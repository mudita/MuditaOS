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
namespace app::relaxation
{
    class RelaxationTimerSelectContract
    {
      public:
        using Range = std::vector<std::chrono::minutes>;
        class View
        {
          public:
            virtual ~View() = default;
        };

        class Presenter : public BasePresenter<RelaxationTimerSelectContract::View>
        {
          public:
            virtual ~Presenter()                                      = default;
            virtual const Range &getTimerValuesRange() const noexcept = 0;
            virtual std::chrono::minutes getCurrentTimerValue() const = 0;
            virtual void setTimerValue(std::chrono::minutes)          = 0;
        };
    };

    class RelaxationTimerSelectPresenter : public RelaxationTimerSelectContract::Presenter
    {
        settings::Settings *settings = nullptr;

        const RelaxationTimerSelectContract::Range &getTimerValuesRange() const noexcept override;
        std::chrono::minutes getCurrentTimerValue() const override;
        void setTimerValue(std::chrono::minutes) override;

      public:
        explicit RelaxationTimerSelectPresenter(settings::Settings *settings);
    };
} // namespace app::relaxation
