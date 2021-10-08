// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BoxLayoutSizeStore.hpp"

namespace gui
{
    bool BoxLayoutSizeStore::store(Item *it, Size size)
    {
        if (it == nullptr) {
            return false;
        }
        grants[it] = size;
        return true;
    }

    Size BoxLayoutSizeStore::get(Item *it)
    {
        Size size = {0, 0};
        if (it == nullptr) {
            return size;
        }
        auto found = grants.find(it);
        if (found != std::end(grants)) {
            return found->second;
        }
        return {0, 0};
    }

    bool BoxLayoutSizeStore::isSizeSmaller(Item *item, Size size, Axis axis)
    {
        return size.get(axis) < get(item).get(axis);
    }

    void BoxLayoutSizeStore::setReleaseSpaceFlag(const bool &val)
    {
        releaseSpace = val;
    }

    bool BoxLayoutSizeStore::getReleaseSpaceFlag()
    {
        return releaseSpace;
    }

} // namespace gui
