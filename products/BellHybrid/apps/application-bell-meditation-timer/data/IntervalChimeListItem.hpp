// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/widgets/BellSideListItem.hpp>

#include "ScroolSpinner.hpp"

namespace gui
{
    class Label;

    class IntervalChimeListItem : public BellSideListItem
    {
        std::function<void(int)> onIndexChangedCallback{nullptr};
        ScroolSpinner *spinner   = nullptr;
        Label *bottomDescription = nullptr;

        void createSpinner(int size, ScroolSpinner::OnGetTitleCallback getTitleCB);
        void createBottomDescription();
        void registerCallbacks();

      public:
        IntervalChimeListItem(int size, ScroolSpinner::OnGetTitleCallback getTitleCB);

        auto setOnIndexChanged(std::function<void(int)> cb) -> void;
        [[nodiscard]] int getSpinnerIndex() const noexcept;
        void setSpinnerIndex(int index);
    };
} // namespace gui
