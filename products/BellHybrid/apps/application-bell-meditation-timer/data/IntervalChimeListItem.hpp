// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/widgets/BellSideListItem.hpp>
#include <apps-common/widgets/spinners/Spinners.hpp>

namespace gui
{
    class Label;

    class IntervalChimeListItem : public BellSideListItem
    {
        UTF8Spinner *spinner     = nullptr;
        Label *bottomDescription = nullptr;
        std::vector<std::string> titles;

        void createSpinner();
        void createBottomDescription();
        void registerCallbacks();

      public:
        IntervalChimeListItem(std::vector<std::string> titles);

        [[nodiscard]] int getSpinnerIndex() const;
        void setSpinnerIndex(int index);
    };
} // namespace gui
