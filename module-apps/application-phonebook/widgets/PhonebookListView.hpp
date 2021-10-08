// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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

        UTF8 labelMark           = "";
        UTF8 previousLabelMark   = "";
        bool previousItemIsLabel = false;

        void addItemsOnPage() override;
        void addLabelMarker(PhonebookItem *item);

      public:
        PhonebookListView(
            Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h, std::shared_ptr<ListItemProvider> prov);
    };

} /* namespace gui */
