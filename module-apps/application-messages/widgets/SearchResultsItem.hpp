#pragma once

#include <ListItem.hpp>
#include <Label.hpp>

namespace gui
{
    class SearchResultsItem : public ListItem
    {
        Item *layout   = nullptr;
        Label *title   = nullptr;
        Label *date    = nullptr;
        Label *preview = nullptr;

      public:
        SearchResultsItem();
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
        void setTitle(const UTF8 &text);
        void setDate(const UTF8 &text);
        void setPreview(const UTF8 &text);
    };

} /* namespace gui */
