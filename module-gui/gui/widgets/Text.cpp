#include <iterator>

#include "../core/Font.hpp"
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
    Text::Text(Item *parent,
               const uint32_t &x,
               const uint32_t &y,
               const uint32_t &w,
               const uint32_t &h,
               const UTF8 &text,
               ExpandMode expandMode,
               TextType textType)
        : Rect(parent, x, y, w, h), lines(this), expandMode{expandMode}, textType{textType}
    {

        setPenWidth(style::window::default_border_no_focus_w);
        setPenFocusWidth(style::window::default_border_focus_w);
        font = FontManager::getInstance().getFont(style::window::font::small);
        buildDocument(text);

        setBorderColor(gui::ColorFullBlack);
        setEdges(RectangleEdgeFlags::GUI_RECT_ALL_EDGES);
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
    {
    }

    void Text::setUnderline(bool underline)
    {
    }

    void Text::setText(const UTF8 &text)
    {
        debug_text("setText: %s", text.c_str());
        setText(std::make_unique<TextDocument>(textToTextBlocks(text, font, TextBlock::End::None)));
    }

    void Text::setText(std::unique_ptr<TextDocument> &&document)
    {
        buildDocument(std::move(document));
    }

    void Text::addText(const UTF8 &text)
    {
        if (text.length() == 0) {
            return;
        }
        if (document->isEmpty()) {
            addText(TextBlock(text, font, TextBlock::End::None));
        }
        else {
            *cursor << text;
            drawLines();
        }
    }

    void Text::addText(TextBlock text)
    {
        debug_text("adding text...");
        document->append(std::move(text));
        buildCursor();
        drawLines();
    }

    void Text::clear()
    {
        buildDocument("");
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
        Font *newFont = FontManager::getInstance().getFont(fontName);
        font          = newFont;
    }

    void Text::setFont(Font *fontName)
    {
        font = fontName;
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

        debug_text("not handled: %s", evt.str().c_str());

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
        // if margins are smaller than radius update the margins
        if (margins.left < value)
            margins.left = value;
        if (margins.right < value)
            margins.right = value;
    }

    bool Text::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        Rect::onDimensionChanged(oldDim, newDim);
        return true;
    }

    bool Text::handleNavigation(const InputEvent &inputEvent)
    {
        if (!inputEvent.isShortPress()) {
            return false;
        }
        if (isMode(EditMode::SCROLL) && (inputEvent.is(KeyCode::KEY_LEFT) || inputEvent.is(KeyCode::KEY_RIGHT))) {
            debug_text("Text in scroll mode ignores left/right navigation");
        }
        auto ret = cursor->move(inputToNavigation(inputEvent));
        debug_text("move: %s", c_str(ret));
        if (ret == TextCursor::Move::Start || ret == TextCursor::Move::End) {
            debug_text("scrolling needs implementing");
            return false;
        }
        if (ret != TextCursor::Move::Error) {
            return true;
        }
        return false;
    }

    bool Text::handleEnter()
    {
        return false;
    }

    bool Text::addChar(uint32_t utf_value)
    {
        assert(cursor);
        cursor->addChar(utf_value);
        auto block = document->getBlock(cursor);
        if (block == nullptr) {
            LOG_ERROR("No block after add char!");
            return false;
        }
        debug_text("len: %d font: %s",
                   block->length(),
                   block->getFont() == nullptr ? "no font" : block->getFont()->getName().c_str());
        return true;
    }

    void Text::drawCursor()
    {
        cursor->updateView();
    }

    void Text::drawLines()
    {
        lines.erase();

        auto sizeMinusMargin = [&](Axis axis) {
            auto size   = area(Area::Max).size(axis);
            auto margin = margins.getSumInAxis(axis) + padding.getSumInAxis(axis);
            if (size <= margin) {
                size = 0;
            }
            else {
                size -= margin;
            }
            return size;
        };

        uint32_t w           = sizeMinusMargin(Axis::X);
        uint32_t h           = sizeMinusMargin(Axis::Y);
        auto line_y_position = margins.top;
        auto cursor          = 0;

        debug_text("--> START drawLines: {%" PRIu32 ", %" PRIu32 "}", w, h);

        auto line_x_position = margins.left;
        do {
            auto text_line = gui::TextLine(document.get(), cursor, w);
            cursor += text_line.length();

            if (text_line.length() == 0) {
                debug_text("cant show more text from this document");
                break;
            }
            if (line_y_position + text_line.height() > h) { // no more space for next line
                debug_text("no more space for next text_line: %d + %" PRIu32 " > %" PRIu32,
                           line_y_position,
                           text_line.height(),
                           h);
                line_y_position += text_line.height();
                break;
            }

            // for each different text which fits in line, addWidget last - to not trigger callbacks to parent
            // on resizes while not needed, after detach there can be no `break` othervise there will be leak - hence
            // detach
            lines.emplace(std::move(text_line));
            auto &line = lines.last();
            line.setPosition(line_x_position, line_y_position);
            line.setParent(this);
            line.align(alignment, w);

            line_y_position += line.height();

            debug_text_lines("debug text drawing: \n start cursor: %d line length: %d end cursor %d : document length "
                             "%d \n x: %d, y: %d \n%s",
                             cursor - lines.last().length(),
                             lines.last().length(),
                             cursor,
                             document->getText().length(),
                             line_x_position,
                             line_y_position,
                             [&]() -> std::string {
                                 std::string text = document->getText();
                                 return std::string(text.begin() + cursor - lines.last().length(),
                                                    text.begin() + cursor);
                             }()
                                          .c_str());
        } while (true);

        // silly case resize - there request space and all is nice
        // need to at least erase last line if it wont fit
        // should be done on each loop
        {
            uint16_t h_used = line_y_position + margins.bottom;
            uint16_t w_used = lines.maxWidth();
            if (lines.size() == 0) {
                debug_text("No lines to show, try to at least fit in cursor");
                if (font != nullptr) {
                    h_used += font->info.line_height;
                    w_used += TextCursor::default_width;
                    debug_text("epty line height: %d", h_used);
                }
            }
            if (h_used != area(Area::Normal).size(Axis::Y) || w_used != area(Area::Normal).size(Axis::X)) {
                debug_text("size request: %d %d", w_used, h_used);
                auto [w, h] = requestSize(w_used, h_used);
                LOG_INFO("size granted: {%" PRIu32 ", %" PRIu32 "}", w, h);
                // if last wont fit lines.eraseLast();
                // break;
            }
        }

        debug_text("<- END\n");
    }

    void Text::setMargins(const Margins &margins)
    {
        this->margins = margins;
    }

    void Text::setAlignment(const Alignment _alignment)
    {
        alignment = _alignment;
    }

    std::list<DrawCommand *> Text::buildDrawList()
    {
        // we can't build elements to show just before showing.
        // why? because we need to know if these elements fit in
        // if not -> these need to call resize, resize needs to be called prior to buildDrawList
        drawCursor();
        debug_text_lines("parent area: %s, elements area: %s",
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
        debug_text_cursor("cursor visibility: %d position: %s", cursor->visible, cursor->area().str().c_str());
        return Rect::buildDrawList();
    }

    void Text::buildDocument(const UTF8 &text)
    {
        buildDocument(std::make_unique<TextDocument>(textToTextBlocks(text, font, TextBlock::End::None)));
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
        cursor = new TextCursor(this, document.get());
    }

    void Text::showCursor(bool focus)
    {
        if (focus) {
            cursor->setVisible(isMode(EditMode::EDIT));
        }
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
        if (document->isEmpty() || !isMode(EditMode::EDIT)) {
            return false;
        }
        if (inputEvent.isShortPress() && inputEvent.is(key_signs_remove)) {
            if (removeChar()) {
                drawLines();
            }
            return true;
        }
        return false;
    }

    bool Text::handleAddChar(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortPress() == false || !isMode(EditMode::EDIT)) {
            return false;
        }

        auto code = translator.handle(inputEvent.key, mode ? mode->get() : "");

        debug_text("%s times: %" PRIu32, inputEvent.str().c_str(), translator.getTimes());

        if (code != KeyProfile::none_key) {
            /// if we have multi press in non digit mode - we need to replace char and put next char from translator
            if (!mode->is(InputMode::digit) && translator.getTimes() > 0) {
                removeChar();
            }
            addChar(code);
            drawLines();
            return true;
        }
        return false;
    }

    /// changes integer value to ascii int value (with no additional checks)
    char intToAscii(int val)
    {
        return val + '0';
    }

    bool Text::handleDigitLongPress(const InputEvent &inputEvent)
    {
        if (!inputEvent.isLongPress()) {
            return false;
        }
        auto val = toNumeric(inputEvent.keyCode);
        if (val != InvalidNumericKeyCode) {
            addChar(intToAscii(val));
            drawLines();
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

} /* namespace gui */
