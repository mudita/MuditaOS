// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SearchResultsItem.hpp"

namespace gui
{
    void SearchResultsItem::setContact(const UTF8 &text)
    {
        if (contact != nullptr) {
            contact->setText(text);
        }
    }
    void SearchResultsItem::setTimestamp(const UTF8 &text)
    {
        if (timestamp != nullptr) {
            timestamp->setText(text);
        }
    }
    void SearchResultsItem::setPreview(const UTF8 &text)
    {
        if (snippet != nullptr) {
            snippet->setText(text);
        }
    }

} /* namespace gui */
