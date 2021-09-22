// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once
#include <widgets/BellSideListItem.hpp>

namespace gui
{
    class Spinner;

    class BGSoundsVolumeListItem : public BellSideListItem
    {
        Spinner *spinner = nullptr;

        void createSpinner();
        void registerCallbacks();

      public:
        BGSoundsVolumeListItem();

        [[nodiscard]] int getSpinnerValue() const noexcept;
        void setSpinnerValue(int value);
    };
} // namespace gui
