/*
 * BottomBar.hpp
 *
 *  Created on: 13 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_BOTTOMBAR_HPP_
#define MIDDLEWARES_GUI_WIDGETS_BOTTOMBAR_HPP_

#include "Rect.hpp"
#include "Label.hpp"
#include "utf8/UTF8.hpp"

namespace gui
{

    /// Footer for most design windows
    ///
    /// Consists of 3 areas right now `[ left  ] [ center ] [ right ]`
    /// these areas most of times inform user about possibility to execute action with keys
    /// left is left action key, center describes center (enter) key, and right right action key
    /// depending on screen selected elements will be visible
    /// @note when in Text widget it will show input mode if enabled in [ left ] area
    class BottomBar : public Rect
    {
      public:
        enum class Side
        {
            LEFT = 0x01,
            CENTER,
            RIGHT
        };

      protected:
        bool stored = false;
        UTF8 storedLeft;
        UTF8 storedCenter;
        UTF8 storedRight;
        Label *left   = nullptr;
        Label *center = nullptr;
        Label *right  = nullptr;
        gui::Label *prepareLabel(BottomBar::Side side);
        Label *getSide(BottomBar::Side side);
      public:
        BottomBar();
        BottomBar(Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
        virtual ~BottomBar();

        void setActive(BottomBar::Side side, bool active);
        void setText(BottomBar::Side side, const UTF8 &str, bool active = true);
        UTF8 getText(BottomBar::Side side);

        void store();
        void restore();

        // virtual methods from Item
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim);
    };

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_BOTTOMBAR_HPP_ */
