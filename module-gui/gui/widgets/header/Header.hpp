// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <BoxLayout.hpp>
#include <Label.hpp>
#include <Image.hpp>

namespace gui::header
{
    enum class NavigationIndicator
    {
        IceBox,
        AddElementBox,
        SearchBox
    };

    class Header : public HBox
    {
      private:
        /// Pointer to the left horizontal box
        HBox *leftBox = nullptr;

        /// Pointer to the central horizontal box
        HBox *centralBox = nullptr;

        /// Pointer to the right horizontal box
        HBox *rightBox = nullptr;

        Label *title = nullptr;

        void createTitle(const UTF8 &text);
        void createIceBox();
        void createAddElementBox();
        void createSearchBox();
        void hideOuterBoxes();
        void showOuterBoxes();

      public:
        Header(Item *parent, Position x, Position y, Length w, Length h);
        void setTitle(const UTF8 &text);
        [[nodiscard]] UTF8 getTitle();
        void setTitleVisibility(bool visibility);
        void navigationIndicatorAdd(NavigationIndicator indicator);
        void navigationIndicatorRemove(NavigationIndicator indicator);
        [[nodiscard]] bool navigationIndicatorVisible(NavigationIndicator indicator);
    };
} // namespace gui::header
