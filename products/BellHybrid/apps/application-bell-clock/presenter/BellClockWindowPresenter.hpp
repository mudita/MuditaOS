// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "models/DeepRefreshInterval.hpp"

#include <apps-common/BasePresenter.hpp>
#include <common/models/TimeModel.hpp>
#include <gui/Common.hpp>

#include <memory>

namespace app
{
    class ApplicationCommon;
    class AbstractTimeModel;

    namespace clock::models
    {
        class DeepRefreshInterval;
    }
}

namespace gui
{
    class AppWindow;
    class ListItemProvider;
} // namespace gui

namespace app::bell_clock
{
    class View
    {
      public:
        virtual ~View() noexcept = default;
        virtual void setTime(std::time_t time) = 0;
    };

    class AbstractClockPresenter : public BasePresenter<View>
    {
      public:
        virtual ~AbstractClockPresenter() noexcept = default;
        virtual gui::RefreshModes handleUpdateTimeEvent() = 0;
    };

    class BellClockWindowPresenter : public AbstractClockPresenter
    {
      public:
        BellClockWindowPresenter(std::unique_ptr<AbstractTimeModel> timeModel,
                                 clock::models::DeepRefreshInterval &deepRefreshModel);
        gui::RefreshModes handleUpdateTimeEvent() override;

        void updateTime();

      private:
        gui::RefreshModes handleCyclicDeepRefresh();

        std::unique_ptr<AbstractTimeModel> timeModel;
        clock::models::DeepRefreshInterval &deepRefreshModel;
    };
} // namespace app::bell_clock
