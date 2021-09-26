// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/widgets/BellSideListItem.hpp>

namespace gui
{
    class Spinner;
    class Label;

    class MeditationTimerListItem : public BellSideListItem
    {
        std::function<void(int)> onValueChangedCallback{nullptr};
        Spinner *spinner         = nullptr;
        Label *bottomDescription = nullptr;

        void createSpinner();
        void createBottomDescription();
        void registerCallbacks();

      public:
        MeditationTimerListItem();

        auto setOnValueChanged(std::function<void(int)> cb) -> void;
        [[nodiscard]] int getSpinnerValue() const noexcept;
        void setSpinnerValue(int value);
    };
} // namespace gui
