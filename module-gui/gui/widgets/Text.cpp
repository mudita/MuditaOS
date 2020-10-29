// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <iterator>

#include "Common.hpp"
#include "Ellipsis.hpp"
#include "InputEvent.hpp"
#include "InputMode.hpp"
#include "Item.hpp"
#include "Text.hpp"
#include "TextBlock.hpp"
#include "TextConstants.hpp"
#include "TextCursor.hpp"
#include "TextDocument.hpp"
#include "TextLine.hpp"
#include "TextParse.hpp"
#include "log/log.hpp"
#include "utf8/UTF8.hpp"
#include "vfs.hpp"
#include <Style.hpp>
#include <cassert>
#include <FontManager.hpp>
#include <RawFont.hpp>
#include <RichTextParser.hpp>
#include "Lines.hpp"

#if DEBUG_GUI_TEXT == 1
#define debug_text(...) LOG_DEBUG(__VA_ARGS__)
#else
#define debug_text(...)
#endif
#if DEBUG_GUI_TEXT_LINES == 1
#define debug_text_lines(...) LOG_DEBUG(__VA_ARGS__)
#else
#define debug_text_lines(...)
#endif
#if DEBUG_GUI_TEXT_CURSOR == 1
#define debug_text_cursor(...) LOG_DEBUG(__VA_ARGS__)
#else
#define debug_text_cursor(...)
#endif

namespace gui
{
    /// changes integer value to ascii int value (with no additional checks)
    char intToAscii(int val)
    {
        return val + '0';
    }

    Text::Text(Item *parent,
               const uint32_t &x,
               const uint32_t &y,
               const uint32_t &w,
               const uint32_t &h,
               const UTF8 &text,
               ExpandMode expandMode,
               TextType textType)
        : Rect(parent, x, y, w, h), expandMode{expandMode}, textType{textType},
          format(FontManager::getInstance().getFont(style::window::font::small))
    {
        lines = std::make_unique<Lines>(this);

        alignment = style::text::defaultTextAlignment;

        setPenWidth(style::window::default_border_no_focus_w);
        setPenFocusWidth(style::window::default_border_focus_w);
        buildDocument(text);

        setBorderColor(gui::ColorFullBlack);
        setEdges(RectangleEdge::All);
    }

    Text::Text() : Text(nullptr, 0, 0, 0, 0)
    {}

    Text::~Text()
    {
        if (mode != nullptr) {
            delete mode;
        }
    }

    void Text::setEditMode(EditMode new_mode)
    {
        if (new_mode != editMode) {
            editMode = new_mode;
        }
    }

    void Text::setTextType(TextType type)
    {}

    void Text::setUnderline(const bool val)
    {
        if (underline != val) {
            underline = val;
            drawLines();
        }
    }

    void Text::setText(const UTF8 &text)
    {
        debug_text("setText: %s", text.c_str());
        /// TODO here should be format passed
        setText(std::make_unique<TextDocument>(textToTextBlocks(text, format.getFont(), TextBlock::End::None)));
    }

    void Text::setText(std::unique_ptr<TextDocument> &&doc)
    {
        buildDocument(std::move(doc));
        onTextChanged();
    }

    void Text::setTextChangedCallback(TextChangedCallback &&callback)
    {
        textChangedCallback = std::move(callback);
    }

    void Text::addText(const UTF8 &text)
    {
        if (text.length() == 0) {
            return;
        }
        *cursor << text;
        onTextChanged();
        drawLines();
    }

    void Text::addText(TextBlock text)
    {
        *cursor << text;
        onTextChanged();
        drawLines();
    }

    void Text::setRichText(const UTF8 &text)
    {
        setText("");
        addRichText(text);
    }

    void Text::addRichText(const UTF8 &text)
    {
        auto tmp_document = text::RichTextParser().parse(text, &format);
        if (tmp_document->isEmpty()) {
            LOG_ERROR("Nothing to parse/parser error in rich text: %s", text.c_str());
            addText(text); // fallback
        }
        for (auto block : tmp_document->getBlockCursor(0)) {
            *cursor << block;
        }
        drawLines();
    }

    void Text::clear()
    {
        buildDocument("");
    }

    bool Text::isEmpty()
    {
        return document->isEmpty();
    }

    UTF8 Text::getText()
    {
        return document->getText();
    }

    bool Text::saveText(UTF8 path)
    {
        if (auto file = vfs.fopen(path.c_str(), "wb")) {
            auto text = getText();
            vfs.fwrite(text.c_str(), text.length(), text.length(), file);
            vfs.fclose(file);
            return true;
        }
        return false;
    }

    void Text::setFont(const UTF8 &fontName)
    {
        RawFont *newFont = FontManager::getInstance().getFont(fontName);
        format.setFont(newFont);
        buildCursor();
    }

    void Text::setFont(RawFont *font)
    {
        format.setFont(font);
        buildCursor();
    }

    std::tuple<NavigationDirection, uint32_t> scrollView(const TextCursor &cursor)
    {
        uint32_t scrolledLines = 1;
        return {NavigationDirection::UP, scrolledLines};
    }

    bool Text::onInput(const InputEvent &evt)
    {
        if (Rect::onInput(evt)) {
            debug_text("Rect::onInput");
            return true;
        }
        if (handleRotateInputMode(evt)) {
            debug_text("handleRotateInputMode");
            return true;
        }
        if (handleRestoreInputModeUI(evt)) {
            debug_text("handleRestoreInputModeUI");
            return true;
        }
        if (handleSelectSpecialChar(evt)) {
            debug_text("handleSelectSpecialChar");
            return true;
        }
        if (handleActivation(evt)) {
            debug_text("handleActivation");
            return true;
        }
        if (handleNavigation(evt)) {
            debug_text("handleNavigation");
            return true;
        }
        if (handleBackspace(evt)) {
            debug_text("handleBackspace");
            return true;
        }
        if (handleAddChar(evt)) {
            debug_text("handleAddChar");
            return true;
        }
        if (handleDigitLongPress(evt)) {
            debug_text("handleDigitLongPress");
            return true;
        }

        return false;
    }

    bool Text::onFocus(bool state)
    {
        if (state != focus && state == true) {
            drawLines();
        }
        showCursor(state);
        return true;
    }

    void Text::setRadius(int value)
    {
        Rect::setRadius(value);
        // if padding are smaller than radius update the padding
        if (padding.left < value)
            padding.left = value;
        if (padding.right < value)
            padding.right = value;
    }

    void Text::setPadding(const Padding &value)
    {
        if (padding != value) {
            padding = value;
            buildCursor();
            drawLines();
        }
    }

    void Text::setAlignment(const Alignment &value)
    {
        if (alignment != value) {
            alignment = value;
            drawLines();
        }
    }

    bool Text::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        Rect::onDimensionChanged(oldDim, newDim);
        drawLines();
        return true;
    }

    bool Text::handleNavigation(const InputEvent &inputEvent)
    {
        if (!inputEvent.isShortPress()) {
            return false;
        }

        //        if (lines->checkNavigationBounds(*cursor, inputEvent) == InputBound::CAN_MOVE) {

        if (isMode(EditMode::SCROLL) && (inputEvent.is(KeyCode::KEY_LEFT) || inputEvent.is(KeyCode::KEY_RIGHT))) {
            debug_text("Text in scroll mode ignores left/right navigation");
            return false;
        }
            auto ret = cursor->moveCursor(inputToNavigation(inputEvent));
            debug_text("moveCursor: %s", c_str(ret));
            if (ret == TextCursor::Move::Start || ret == TextCursor::Move::End) {
                debug_text("scrolling needs implementing");
                return false;
            }
            if (ret != TextCursor::Move::Error) {
                return true;
            }
            //        }

            return false;
    }

    bool Text::handleEnter()
    {
        return false;
    }

    bool Text::addChar(uint32_t utf_value)
    {
        cursor->addChar(utf_value);
        auto block = document->getBlock(cursor);

        if (block == nullptr) {
            return false;
        }
        debug_text("value: %d, block len: %d", utf_value, block->length());
        return true;
    }

    void Text::drawCursor()
    {
        cursor->updateView();
    }

    void Text::drawLines()
    {
        lines->erase();

        auto sizeMinusPadding = [&](Axis axis, Area val) {
            auto size = area(val).size(axis);
            if (size <= padding.getSumInAxis(axis)) {
                size = 0;
            }
            else {
                size -= padding.getSumInAxis(axis);
            }
            return size;
        };

        // Check if Parent does not have max size restrictions.
        if (this->parent != nullptr) {
            area(Area::Max).w = parent->area(Area::Max).w != 0 ? std::min(parent->area(Area::Max).w, area(Area::Max).w)
                                                               : area(Area::Max).w;
            area(Area::Max).h = parent->area(Area::Max).h != 0 ? std::min(parent->area(Area::Max).h, area(Area::Max).h)
                                                               : area(Area::Max).h;
        }

        Length w = sizeMinusPadding(Axis::X, Area::Max);
        Length h = sizeMinusPadding(Axis::Y, Area::Max);

        auto lineYPosition = padding.top;

        BlockCursor drawCursor(cursor->getDocument(), 0, 0, getTextFormat().getFont());

        debug_text("--> START drawLines: {%" PRIu32 ", %" PRIu32 "}", w, h);

        LOG_ERROR("ZACZYNAM RYSOWANIE LINI!!!!");

        auto lineXPosition = padding.left;
        do {
            auto textLine = gui::TextLine(drawCursor, w);

            LOG_DEBUG("Jaka ma długość %d", textLine.length());

            if (textLine.length() == 0 && textLine.linesEnd) {
                debug_text("cant show more text from this document");
                break;
            }

            if (lineYPosition + textLine.height() > h) { // no more space for next line
                debug_text("no more space for next text_line: %d + %" PRIu32 " > %" PRIu32,
                           lineYPosition,
                           textLine.height(),
                           h);
                lineYPosition += textLine.height();
                break;
            }

            // for each different text which fits in line, addWidget last - to not trigger callbacks to parent
            // on resizes while not needed, after detach there can be no `break` othervise there will be leak - hence
            // detach
            lines->emplace(std::move(textLine));
            auto &line = lines->last();

            line.setPosition(lineXPosition, lineYPosition);
            line.setParent(this);

            lineYPosition += line.height();

            LOG_DEBUG("ROBIE NOWA LINIE !!!!!!! A mamy ich %lu", lines->size());

            debug_text("debug text drawing: \n cursor pos: %d line length: %d : document length "
                       "%d \n x: %d, y: %d \n%s",
                       drawCursor.getPosition(),
                       lines->last().length(),
                       document->getText().length(),
                       lineXPosition,
                       lineYPosition,
                       [&]() -> std::string {
                           std::string text = document->getText();
                           return text;
                       }()
                                    .c_str());
        } while (true);

        // silly case resize - there request space and all is nice
        // need to at least erase last line if it wont fit
        // should be done on each loop
        {
            uint16_t hUsed = lineYPosition + padding.bottom;
            uint16_t wUsed = lines->maxWidth() + padding.getSumInAxis(Axis::X);

            if (lines->size() == 0) {
                debug_text("No lines to show, try to at least fit in cursor");
                if (format.getFont() != nullptr && lineYPosition < format.getFont()->info.line_height) {
                    hUsed = format.getFont()->info.line_height;
                    wUsed = TextCursor::default_width;
                    debug_text("empty line height: %d", hUsed);
                }
            }

            if (hUsed != area(Area::Normal).size(Axis::Y) || wUsed != area(Area::Normal).size(Axis::X)) {
                debug_text("size request: %d %d", wUsed, hUsed);
                auto [w, h] = requestSize(wUsed, hUsed);

                if (h < hUsed) {
                    debug_text("No free height for text!");
                }
            }

            lines->linesHAlign(sizeMinusPadding(Axis::X, Area::Normal));
            lines->linesVAlign(sizeMinusPadding(Axis::Y, Area::Normal));

            debug_text("<- END\n");
        }
    }

    std::list<DrawCommand *> Text::buildDrawList()
    {
        // we can't build elements to show just before showing.
        // why? because we need to know if these elements fit in
        // if not -> these need to call resize, resize needs to be called prior to buildDrawList
        drawCursor();
        debug_text("parent area: %s, elements area: %s",
                   area().str().c_str(),
                   [&]() -> std::string {
                       std::string str;
                       for (auto el : children) {
                           if (auto label = dynamic_cast<gui::Label *>(el)) {
                               str += "area: " + label->area().str() + std::string(label->getText()) + " ";
                           }
                       }
                       return str.c_str();
                   }()
                                .c_str());
        return Rect::buildDrawList();
    }

    void Text::buildDocument(const UTF8 &text)
    {
        buildDocument(std::make_unique<TextDocument>(textToTextBlocks(text, format.getFont(), TextBlock::End::None)));
    }

    void Text::buildDocument(std::unique_ptr<TextDocument> &&document_moved)
    {
        document = std::move(document_moved);
        debug_text("document text: %s", document->getText().c_str());
        buildCursor();
        drawLines();
    }

    void Text::buildCursor()
    {
        erase(cursor);
        cursor = new TextLineCursor(this);
        cursor->setAlignment(this->getAlignment());
        cursor->setMargins(this->getPadding());
        showCursor(focus);
    }

    void Text::showCursor(bool focus)
    {
        cursor->setVisible(focus && isMode(EditMode::EDIT));
    }

    bool Text::handleRotateInputMode(const InputEvent &inputEvent)
    {
        if (mode != nullptr && inputEvent.isShortPress() && inputEvent.keyCode == gui::KeyCode::KEY_AST) {
            mode->next();
            return true;
        }
        return false;
    }

    bool Text::handleRestoreInputModeUI(const InputEvent &inputEvent)
    {
        if (mode != nullptr && inputEvent.isKeyRelease()) {
            mode->show_restore();
        }
        return false;
    }

    bool Text::handleSelectSpecialChar(const InputEvent &inputEvent)
    {

        if (mode != nullptr && inputEvent.isLongPress() && inputEvent.keyCode == gui::KeyCode::KEY_AST) {
            mode->select_special_char();
            return true;
        }
        return false;
    }

    bool Text::handleActivation(const InputEvent &inputEvent)
    {
        return inputEvent.isShortPress() && inputEvent.is(KeyCode::KEY_AST) && Rect::onActivated(nullptr);
    }

    bool Text::handleBackspace(const InputEvent &inputEvent)
    {
        if (!isMode(EditMode::EDIT)) {
            return false;
        }
        //        if (lines->checkRemovalBounds(*cursor, inputEvent) == InputBound::CAN_REMOVE) {
        if (inputEvent.isShortPress() && inputEvent.is(key_signs_remove)) {
            if (!document->isEmpty() && removeChar()) {
                onTextChanged();
                drawLines();
            }
            return true;
        }
        //        }
        return false;
    }

    bool Text::handleAddChar(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortPress() == false || !isMode(EditMode::EDIT)) {
            return false;
        }

        if (lines->checkAdditionBounds(*cursor, inputEvent) == InputBound::CAN_ADD) {

            auto code = translator.handle(inputEvent.key, mode ? mode->get() : "");
            debug_text("handleAddChar %d -> Begin", code);
            debug_text("%s times: %" PRIu32, inputEvent.str().c_str(), translator.getTimes());

            if (code != KeyProfile::none_key) {
                /// if we have multi press in non digit mode - we need to replace char and put next char from translator
                if (!(mode->is(InputMode::digit) || (mode->is(InputMode::phone))) && translator.getTimes() > 0) {
                    removeChar();
                }
                addChar(code);
                onTextChanged();
                drawLines();

                debug_text("handleAddChar -> End(true)");

                return true;
            }
        }

        debug_text("handleAdChar -> End(false)");
        return false;
    }

    bool Text::handleDigitLongPress(const InputEvent &inputEvent)
    {
        if (!inputEvent.isLongPress()) {
            return false;
        }
        auto val = toNumeric(inputEvent.keyCode);
        if (val != InvalidNumericKeyCode) {
            addChar(intToAscii(val));
            onTextChanged();
            drawLines();
            return true;
        }
        return false;
    }

    bool Text::removeChar()
    {
        if (!document->isEmpty()) {
            cursor->removeChar();
            return true;
        }
        return false;
    }

    void Text::onTextChanged()
    {
        if (textChangedCallback) {
            textChangedCallback(*this, getText());
        }
    }

} /* namespace gui */
