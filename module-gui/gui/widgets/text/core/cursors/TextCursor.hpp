// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <TextConstants.hpp>
#include <core/TextDocument.hpp>

#include <Common.hpp>
#include <Rect.hpp>

namespace gui
{
    class Text;
    class TextDocument;

    /// TextCursor - element joining Visible cursor element on screen ( via Rect )
    /// movement on TextDocument ( via BlockCursor )
    /// and position in gui::Text::Lines shown on screen
    class TextCursor : public Rect, public BlockCursor
    {
      protected:
        Text *text                              = nullptr;
        CursorStartPosition cursorStartPosition = CursorStartPosition::DocumentEnd;
        unsigned int onScreenPosition           = 0;

      public:
        static const unsigned int defaultWidth;
        enum class Move
        {
            Start, /// we are text `0`
            End,   /// we hit end of document
            Up,    /// we moved up a line
            Down,  /// we moved down a line
            Left,  /// we moved left inline
            Right, /// we moved right inline

            Error, /// error - now not implemented
        };

        explicit TextCursor(gui::Text *parent, unsigned int pos = text::npos, unsigned int block = text::npos);
        TextCursor() = delete;

        /// Up Down - end of line movement like in vi
        /// - moves on TextBlock ( TextDocument really... ) cursor to know where we are
        /// - moves gui::Text relative to TextDocument `pos_on_screen` value
        /// - informs that we changed line when needed - TODO think about it better... ( and if it's needed...?)
        /// - with_update - updates position in parent ( if false not - means we handled it already with i.e. addChar or
        /// removeChar)
        virtual Move moveCursor(NavigationDirection direction);
        virtual Move moveCursor(NavigationDirection direction, unsigned int n);

        // TODO note to self - here should be too UTF8 char handling, not in document...
        // cursor can pass processing char directly to TextBlock we are interested in...
        // so this should be in BlockCursor in reality
        auto getSelectedLine() -> std::tuple<const TextLine *, unsigned int, unsigned int>;
        void updateView();

        void addChar(uint32_t utf_val) override;
        TextCursor &operator<<(const UTF8 &);
        TextCursor &operator<<(const TextBlock &);
        bool removeChar() override;

        auto setCursorStartPosition(CursorStartPosition val) -> void;

        [[nodiscard]] auto getOnScreenPosition() const
        {
            return onScreenPosition;
        }
        [[nodiscard]] auto getAbsolutePosition() const -> unsigned int;
    };
} // namespace gui

const char *c_str(enum gui::TextCursor::Move);
