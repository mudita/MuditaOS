#pragma once

#include "Common.hpp"

namespace gui
{

    class Item;

    /// class for use with layouts
    /// it's meant to store requested sizes from Layout to use in resizes
    class LayoutSizeStore
    {
      public:
        virtual ~LayoutSizeStore()                                                     = default;
        virtual auto store(Item *, Size size) -> bool                                  = 0;
        [[nodiscard]] virtual auto get(Item *) -> Size                                 = 0;
        [[nodiscard]] virtual auto isSizeSmaller(Item *, Size size, Axis axis) -> bool = 0;

        virtual void setReleaseSpaceFlag(const bool &val) = 0;
        [[nodiscard]] virtual bool getReleaseSpaceFlag()  = 0;
    };
} // namespace gui
