#pragma once

#include <LayoutSizeStore.hpp>
#include <map>

namespace gui
{
    class BoxLayoutSizeStore : public LayoutSizeStore
    {
        std::map<Item *, Size> grants;

      public:
        bool store(Item *, Size size) override;
        Size get(Item *) override;
    };
} // namespace gui
