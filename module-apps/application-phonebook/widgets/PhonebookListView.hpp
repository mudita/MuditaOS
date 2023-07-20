// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "application-phonebook/widgets/PhonebookItem.hpp"
#include <apps-common/ApplicationCommon.hpp>

#include <ListView.hpp>

namespace gui
{
    class ListItemProvider;

    class PhonebookListView : public ListView
    {
      public:
        PhonebookListView(Item *parent,
                          std::uint32_t x,
                          std::uint32_t y,
                          std::uint32_t w,
                          std::uint32_t h,
                          std::shared_ptr<ListItemProvider> prov);

      private:
        void addItemsOnPage() override;
        void addLabelMarker(PhonebookItem *item);
        std::size_t getSlotsLeft();

        UTF8 labelMark           = "";
        bool previousItemIsLabel = false;
    };
} /* namespace gui */
