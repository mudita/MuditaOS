// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Label.hpp"

namespace gui
{
    Label::Label(Item *parent, Position x, Position y, Length w, Length h, const UTF8 &inputText)
        : TextFixedSize(parent, x, y, w, h)
    {
        setEditMode(EditMode::Print);
        setCursorStartPosition(CursorStartPosition::DocumentBegin);
        setTextType(TextType::SingleLine);
        drawUnderline(false);
        setPenFocusWidth(style::window::default_border_rect_no_focus);
        setPenWidth(style::window::default_border_rect_no_focus);

        setText(inputText);
    }

    uint32_t Label::getTextNeedSpace(const UTF8 &inputText) const noexcept
    {
        return inputText.empty() ? format.getFont()->getPixelWidth(getText())
                                 : format.getFont()->getPixelWidth(inputText);
    }
} /* namespace gui */
