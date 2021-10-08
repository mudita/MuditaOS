// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <LayoutSizeStore.hpp>
#include <map>

namespace gui
{
    class BoxLayoutSizeStore : public LayoutSizeStore
    {
      private:
        /// Flag allowing BoxLayout to push out currently visible elements in order to resize requested element.
        bool releaseSpace = true;

        std::map<Item *, Size> grants;

      public:
        bool store(Item *, Size size) override;
        [[nodiscard]] Size get(Item *) override;
        [[nodiscard]] bool isSizeSmaller(Item *, Size size, Axis axis) override;

        void setReleaseSpaceFlag(const bool &val) override;
        [[nodiscard]] bool getReleaseSpaceFlag() override;
    };
} // namespace gui
