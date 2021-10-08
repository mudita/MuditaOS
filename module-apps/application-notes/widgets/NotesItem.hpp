// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/widgets/Label.hpp>
#include <module-gui/gui/widgets/ListItem.hpp>
#include <module-gui/gui/widgets/TextFixedSize.hpp>

#include <module-db/Interface/NotesRecord.hpp>

namespace gui
{
    class NotesItem : public ListItem
    {
      public:
        explicit NotesItem(std::shared_ptr<NotesRecord> record);

        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;

      private:
        static gui::TextFixedSize *createTextField(Item *parent, const UTF8 &fontName);

        void buildInterface();
        void setSnippet(const UTF8 &noteText);
        void setDateText(std::uint32_t timestamp);

        std::shared_ptr<NotesRecord> note;
        gui::Label *date            = nullptr;
        gui::TextFixedSize *title   = nullptr;
        gui::TextFixedSize *snippet = nullptr;
    };
} // namespace gui
