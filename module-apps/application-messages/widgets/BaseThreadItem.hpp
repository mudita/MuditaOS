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
        gui::Label *preview          = nullptr;

        virtual void onDimensionChangedTop(const BoundingBox &oldDim, const BoundingBox &newDim);
        virtual void onDimensionChangedBottom(const BoundingBox &oldDim, const BoundingBox &newDim);

        void displayNumberImportance(long int importance);

      public:
        BaseThreadItem();
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };
} // namespace gui
