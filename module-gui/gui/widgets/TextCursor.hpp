#pragma once

#include "Common.hpp"
#include "Rect.hpp"
#include "TextConstants.hpp"
#include "TextDocument.hpp"
#include <memory>

namespace gui
{

    class Text;
    class TextDocument;

    /// TextCursor - element joining Visible cursor element on screen ( via Rect )
    /// movement on TextDocument ( via BlockCursor )
    /// and position in gui::Text::Lines shown on screen
    class TextCursor : public Rect, public BlockCursor
    {
        unsigned int pos_on_screen = 0;
        Text *text                 = nullptr;

      public:
        static const unsigned int default_width;
        enum class Move
        {
            Start, /// we are text `0`
            End,   /// we hit end of document
            Up,    /// we moved up a line
            Down,  /// we moved down a line

            InLine, /// no action - passed movement by `0` or we are just somewhere comfty in visible range

            Error, /// error - now not implemented
        };

        TextCursor(gui::Text *parent, gui::TextDocument *document);
        TextCursor() = delete;

        /// Up Down - end of line movement like in vi
        /// - moves on TextBlock ( TextDocument really... ) cursor to know where we are
        /// - moves gui::Text relative to TextDocument `pos_on_screen` value
        /// - informs that we changed line when needed - TODO think about it better... ( and if it's needed...?)
        /// - with_update - updates position in parent ( if false not - means we handled it already with i.e. addChar or
        /// removeChar)
        Move move(NavigationDirection direction);
        void reset();

        // TODO note to self - here should be too UTF8 char handling, not in document...
        // cursor can pass processing char directly to TextBlock we are interested in...
        // so this should be in BlockCursor in reality
        auto getLine() -> std::tuple<const TextLine *, unsigned int, unsigned int>;
        void updateView();

        // TODO this can move our text out of bonds ( and might need calling expand() in Text)
        void addChar(uint32_t utf_val);
        TextCursor &operator<<(const UTF8 &);
        void removeChar();
    };
} // namespace gui

const char *c_str(enum gui::TextCursor::Move);
