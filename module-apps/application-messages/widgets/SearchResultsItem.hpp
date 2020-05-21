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
