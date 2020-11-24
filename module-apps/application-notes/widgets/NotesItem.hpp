// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Label.hpp"
#include "ListItem.hpp"

#include "module-apps/application-notes/model/NotesListModel.hpp"

namespace gui
{
    class NotesItem : public ListItem
    {
      public:
        NotesItem(std::shared_ptr<NotesRecord> record, bool mode24H);

        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;

      private:
        static gui::Label *createEmptyLabel(Item *parent);
        void setSnippet(const UTF8 &noteText);
        void setDateText(std::uint32_t timestamp);

        std::shared_ptr<NotesRecord> note;
        gui::Label *date    = nullptr;
        gui::Label *title   = nullptr;
        gui::Label *snippet = nullptr;
        bool mode24H        = false;
    };
} // namespace gui
