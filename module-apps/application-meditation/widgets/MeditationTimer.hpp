// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ApplicationMeditation.hpp"

#include <gui/widgets/BoxLayout.hpp>
#include <gui/widgets/ProgressBar.hpp>
#include <gui/widgets/Text.hpp>
#include <apps-common/widgets/TimerWithCallbacks.hpp>

namespace app
{
    class ProgressTimerWithBarGraphAndCounter;
}

namespace gui
{
    class MeditationTimer : public HBox
    {

      public:
        MeditationTimer(std::uint32_t x,
                        std::uint32_t y,
                        std::uint32_t width,
                        std::uint32_t height,
                        app::ApplicationMeditation *app,
                        Item *_parent = nullptr);

        [[nodiscard]] auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;

        void setCounterVisible(bool isVisible) noexcept;
        [[nodiscard]] app::TimerWithCallbacks &getTimer() noexcept;
        [[nodiscard]] gui::Progress &getProgress() noexcept;

      private:
        void build();

        app::ApplicationMeditation *application = nullptr;
        CircularProgressBar *progressBar        = nullptr;
        Text *text                              = nullptr;
        std::unique_ptr<app::ProgressTimerWithBarGraphAndCounter> timer;
    };
} // namespace gui
