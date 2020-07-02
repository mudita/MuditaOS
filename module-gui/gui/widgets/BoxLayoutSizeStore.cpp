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
} // namespace gui
