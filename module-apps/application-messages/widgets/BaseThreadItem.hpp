// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Label.hpp"
#include "Image.hpp"
#include "ListItem.hpp"

namespace gui
{
    class BaseThreadItem : public ListItem
    {

      protected:
        gui::Label *contact          = nullptr;
        gui::Label *numberImportance = nullptr;
        gui::Label *timestamp        = nullptr;
        gui::Label *snippetPrefix    = nullptr;
        gui::Label *snippet          = nullptr;

        static gui::Label *createEmptyLabel(Item *parent);

        virtual void onDimensionChangedTop(const BoundingBox &oldDim, const BoundingBox &newDim);
        virtual void onDimensionChangedBottom(const BoundingBox &oldDim, const BoundingBox &newDim);
        void resizeSnippet(const BoundingBox &dimensions, unsigned int leftOffset = 0U);

        void displayNumberImportance(long int importance);

      public:
        BaseThreadItem();
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };
} // namespace gui
