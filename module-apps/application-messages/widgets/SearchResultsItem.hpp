#pragma once

#include <ListItem.hpp>

namespace gui
{
    class SearchResultsItem : public ListItem
    {
      public:
        SearchResultsItem();
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

} /* namespace gui */
