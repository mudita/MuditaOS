// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include "BaseThreadItem.hpp"
#include <Label.hpp>

namespace gui
{
    class SearchResultsItem : public BaseThreadItem
    {
      public:
        void setContact(const UTF8 &text);
        void setTimestamp(const UTF8 &text);
        void setPreview(const UTF8 &text);
    };

} /* namespace gui */
