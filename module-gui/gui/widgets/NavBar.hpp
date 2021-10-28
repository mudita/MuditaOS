// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <ThreeBox.hpp>
#include <Label.hpp>
#include <utf8/UTF8.hpp>

namespace gui::nav_bar
{
    enum class Side
    {
        Left,
        Center,
        Right
    };

    /// Footer for most design windows
    ///
    /// Consists of 3 areas right now `[ left  ] [ center ] [ right ]`
    /// these areas most of times inform user about possibility to execute action with keys
    /// left is left action key, center describes center (enter) key, and right right action key
    /// depending on screen selected elements will be visible
    /// @note when in Text widget it will show input mode if enabled in [ left ] area
    class NavBar : public HThreeBox<HBox, HBox, HBox>
    {
      protected:
        struct Cache
        {
          private:
            struct
            {
                bool isVisible = false;
                UTF8 text;
                bool stored = false;
            } left, center, right;

          public:
            auto get(Side side) -> auto &
            {
                switch (side) {
                case Side::Left:
                    return left;
                case Side::Right:
                    return right;
                case Side::Center:
                    return center;
                }
                return left;
            }
        } cache;

        Label *left   = nullptr;
        Label *center = nullptr;
        Label *right  = nullptr;
        void createLabels();

        void setLeftSideWidth(Length width);
        void setRightSideWidth(Length width);
        bool checkSideOccupied(nav_bar::Side side);
        Length outerSideUsedWidth(nav_bar::Side side);
        void applyOuterSidesWidth(nav_bar::Side side, const UTF8 &str);
        Label *getSide(nav_bar::Side side);

      public:
        NavBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

        void setActive(nav_bar::Side side, bool active);
        bool isActive(nav_bar::Side side);
        void setText(nav_bar::Side side, const UTF8 &str, bool active = true);
        UTF8 getText(nav_bar::Side side);
        void setFont(Side side, const UTF8 &fontName);

        void store();
        void store(Side side);
        void restore();
        void restore(Side side);

        void accept(GuiVisitor &visitor) override;
    };

} // namespace gui::nav_bar
