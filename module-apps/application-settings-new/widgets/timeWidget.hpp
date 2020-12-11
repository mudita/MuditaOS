// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Label.hpp>
#include <Text.hpp>

namespace gui
{
    class TimeWidget : public VBox
    {
      public:
        TimeWidget(Item *parent,
                   const std::string &description,
                   std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                   std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);

        [[nodiscard]] auto getTime() const noexcept -> std::chrono::minutes;
        void setTime(std::chrono::minutes newTime);

      private:
        gui::HBox *hBox         = nullptr;
        Label *descriptionLabel = nullptr;
        Text *hourInput         = nullptr;
        Label *colonLabel       = nullptr;
        Text *minuteInput       = nullptr;
        Label *separatorLabel   = nullptr;
        Label *meridiemInput    = nullptr;

        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;

        void applyFocusCallbacks();
        void applyInputCallbacks();
        [[nodiscard]] auto readTimeFromInputFields() const -> bool;
    };

} /* namespace gui */
