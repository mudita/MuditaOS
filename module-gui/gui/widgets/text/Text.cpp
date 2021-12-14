// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Text.hpp"

#include <parsers/TextParse.hpp>
#include <FontManager.hpp>

#include <Common.hpp>
#include <log/log.hpp>

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
               ExpandMode expandMode,
               TextType textType)
        : Rect(parent, x, y, w, h), expandMode{expandMode}, textType{textType},
          format(FontManager::getInstance().getFont(style::window::font::small))
    {
        lines = std::make_unique<Lines>(this);

        alignment = style::text::defaultTextAlignment;

        setPenWidth(style::window::default_border_no_focus_w);
        setPenFocusWidth(style::window::default_border_focus_w);
        buildCursor();

        setBorderColor(gui::ColorFullBlack);
        setEdges(RectangleEdge::All);

        preBuildDrawListHook = [this](std::list<Command> &commands) { preBuildDrawListHookImplementation(commands); };
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

    void Text::setTextLimitType(TextLimitType limitType, unsigned int val)
    {
        auto it = std::find_if(limitsList.begin(), limitsList.end(), [&limitType](const TextLimit &limitOnList) {
            return limitOnList.limitType == limitType;
        });

        if (it != limitsList.end()) {
            (*it).limitValue = val;
        }
        else {
            limitsList.emplace_back(TextLimit{limitType, val});
        }
    }

    void Text::clearTextLimits()
    {
        limitsList.clear();
    }

    void Text::drawUnderline(bool val)
    {
        if (lines->getUnderLineProperties().draw != val) {
            lines->getUnderLineProperties().draw = val;
            drawLines();
        }
    }

    void Text::setText(const UTF8 &text)
    {
        debug_text("setText: %s", text.c_str());
        setText(std::make_unique<TextDocument>(textToTextBlocks(text, format)));
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

    void Text::addText(const UTF8 &text, AdditionType additionType)
    {
        if (text.length() == 0) {
            return;
        }

        if (additionType == AdditionType::perChar) {
            *cursor << text;
        }
        else if (additionType == AdditionType::perBlock) {
            for (const auto &block : textToTextBlocks(text, format)) {
                *cursor << block;
            }
        }

        onTextChanged();
        drawLines();
    }

    void Text::addText(TextBlock text)
    {
        *cursor << text;
        onTextChanged();
        drawLines();
    }

    void Text::setRichText(const UTF8 &text, text::RichTextParser::TokenMap &&tokenMap)
    {
        setText("");
        addRichText(text, std::move(tokenMap));
    }

    void Text::addRichText(const UTF8 &text, text::RichTextParser::TokenMap &&tokenMap)
    {
        auto tmp_document = text::RichTextParser().parse(text, &format, std::move(tokenMap));

        if (!tmp_document || tmp_document->isEmpty()) {

            debug_text("Nothing to parse/parser error in rich text: %s", text.c_str());
            return addText(text); // fallback
        }
        for (auto block : tmp_document->getBlockCursor(0)) {
            *cursor << block;
        }
        drawLines();
    }

    void Text::clear()
    {
        buildDocument("");
        onTextChanged();
    }

    bool Text::isEmpty()
    {
        return document->isEmpty();
    }

    UTF8 Text::getText() const
    {
        return document->getText();
    }

    void Text::setFont(const UTF8 &fontName)
    {
        RawFont *newFont = FontManager::getInstance().getFont(fontName);

        if (format.getFont() != newFont) {
            format.setFont(newFont);
            buildDocument(getText());
        }
    }

    void Text::setFont(RawFont *font)
    {
        format.setFont(font);
        buildCursor();
    }

    void Text::setMinimumWidthToFitText()
    {
        setMinimumWidthToFitText(getText());
    }

    void Text::setMinimumWidthToFitText(const UTF8 &text)
    {
        if (!text.empty()) {
            auto textToFit = !text::RichTextParser().parse(text, &format)->getText().empty()
                                 ? text::RichTextParser().parse(text, &format)->getText()
                                 : text;

            setMinimumWidth(format.getFont()->getPixelWidth(textToFit) + TextCursor::defaultWidth);
        }
    }

    void Text::setMinimumHeightToFitText(unsigned int linesCount)
    {
        setMinimumHeight(format.getFont()->info.line_height * linesCount);
    }

    auto Text::setCursorStartPosition(CursorStartPosition val) -> void
    {
        // As we destroy cursors starting position information need to be stored in both places
        cursorStartPosition = val;
        cursor->setCursorStartPosition(cursorStartPosition);
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
        if (handleRemovalChar(evt)) {
            debug_text("handleBackspace");
            return true;
        }
        if (handleWholeTextRemoval(evt)) {
            debug_text("handleLongBackspace");
            return true;
        }
        if (handleAddChar(evt)) {
            debug_text("handleAddChar");
            return true;
        }
        if (handleLongPressAddChar(evt)) {
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

    void Text::setColor(Color color)
    {
        if (format.getColor() != color) {
            format.setColor(color);
        }
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

    auto Text::getSizeMinusPadding(Axis axis, Area val) -> Length
    {
        int size = area(val).size(axis);

        if (size <= padding.getSumInAxis(axis)) {
            size = 0;
        }
        else {
            size -= padding.getSumInAxis(axis);
        }
        return size;
    }

    auto Text::applyParentSizeRestrictions() -> void
    {
        // Check if Parent does not have max size restrictions.
        if (this->parent != nullptr) {
            area(Area::Max).w = parent->area(Area::Max).w != 0 ? std::min(parent->area(Area::Max).w, area(Area::Max).w)
                                                               : area(Area::Max).w;
            area(Area::Max).h = parent->area(Area::Max).h != 0 ? std::min(parent->area(Area::Max).h, area(Area::Max).h)
                                                               : area(Area::Max).h;
        }
    }

    auto Text::drawCursor() -> void
    {
        cursor->updateView();
    }

    auto Text::drawLines() -> void
    {
        lines->erase();

        applyParentSizeRestrictions();

        const auto [startDrawBlockNumber, startDrawBlockPosition] = lines->drawStartConditions;
        BlockCursor drawCursor(
            cursor->getDocument(), startDrawBlockPosition, startDrawBlockNumber, getTextFormat().getFont());

        debug_text("--> START drawLines: {%" PRIu32 ", %" PRIu32 "}", w, h);

        lines->draw(drawCursor,
                    getSizeMinusPadding(Axis::X, Area::Max) - TextCursor::defaultWidth,
                    getSizeMinusPadding(Axis::Y, Area::Max),
                    padding.top,
                    padding.left);

        calculateAndRequestSize();

        lines->linesHAlign(getSizeMinusPadding(Axis::X, Area::Normal));
        lines->linesVAlign(getSizeMinusPadding(Axis::Y, Area::Normal));

        debug_text("<- END\n");
    }

    auto Text::calculateAndRequestSize() -> void
    {
        // silly case resize - there request space and all is nice
        // need to at least erase last line if it wont fit
        // should be done on each loop
        {
            Length hUsed = padding.top + lines->linesHeight() + padding.bottom;
            Length wUsed = lines->maxWidth() + padding.getSumInAxis(Axis::X);

            if (lines->size() == 0) {
                debug_text("No lines to show, try to at least fit in cursor");
                if (format.getFont() != nullptr &&
                    padding.top + lines->linesHeight() < format.getFont()->info.line_height) {
                    hUsed = format.getFont()->info.line_height;
                    wUsed = TextCursor::defaultWidth;
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
        }
    }

    void Text::preBuildDrawListHookImplementation(std::list<Command> &commands)
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
    }

    void Text::buildDocument(const UTF8 &text)
    {
        buildDocument(std::make_unique<TextDocument>(textToTextBlocks(text, format)));
    }

    void Text::buildDocument(std::unique_ptr<TextDocument> &&document_moved)
    {
        if (document_moved == nullptr) {
            debug_text("Passed invalid document, returning without setting it");
            return;
        }

        lines->reset();
        document->destroy();
        buildCursor();

        for (const auto &block : document_moved->getBlocks()) {
            *cursor << block;
        }
        debug_text("document text: %s", document->getText().c_str());
        drawLines();
    }

    void Text::buildCursor()
    {
        erase(cursor);
        cursor = new TextLineCursor(this);
        cursor->setCursorStartPosition(cursorStartPosition);
        cursor->setAlignment(this->getAlignment());
        cursor->setMargins(this->getPadding());
        showCursor(focus);
    }

    void Text::showCursor(bool focus)
    {
        cursor->setVisible(focus && isMode(EditMode::Edit));
    }

    auto Text::handleRotateInputMode(const InputEvent &inputEvent) -> bool
    {
        if (mode != nullptr && inputEvent.isShortRelease(gui::KeyCode::KEY_AST)) {
            mode->next();
            return true;
        }
        return false;
    }

    auto Text::handleRestoreInputModeUI(const InputEvent &inputEvent) -> bool
    {
        if (mode != nullptr && inputEvent.isKeyRelease()) {
            mode->show_restore();
        }
        return false;
    }

    auto Text::handleSelectSpecialChar(const InputEvent &inputEvent) -> bool
    {
        if (mode != nullptr && inputEvent.isLongRelease() && inputEvent.is(gui::KeyCode::KEY_AST)) {
            mode->select_special_char();
            return true;
        }
        return false;
    }

    auto Text::handleActivation(const InputEvent &inputEvent) -> bool
    {
        return inputEvent.isShortRelease(KeyCode::KEY_AST) && Rect::onActivated(nullptr);
    }

    auto Text::handleNavigation(const InputEvent &inputEvent) -> bool
    {
        if (!inputEvent.isShortRelease()) {
            return false;
        }

        if (isMode(EditMode::Scroll)) {

            debug_text("Text in scroll mode ignores left/right navigation");
            if (inputEvent.is(KeyCode::KEY_LEFT) || inputEvent.is(KeyCode::KEY_RIGHT)) {
                return false;
            }

            if (inputEvent.is(KeyCode::KEY_DOWN)) {
                return cursor->displayNextLine();
            }

            if (inputEvent.is(KeyCode::KEY_UP)) {
                return cursor->displayPreviousLine();
            }
        }

        if (inputToNavigation(inputEvent) != NavigationDirection::NONE) {

            setCursorStartPosition(CursorStartPosition::Offset);

            auto ret = cursor->moveCursor(inputToNavigation(inputEvent));
            debug_text("moveCursor: %s", c_str(ret));

            if (ret == TextCursor::Move::Start || ret == TextCursor::Move::End) {
                debug_text("scrolling needs implementing");
                return false;
            }
            if (ret != TextCursor::Move::Error) {
                return true;
            }
        }

        return false;
    }

    bool Text::handleRemovalChar(const InputEvent &inputEvent)
    {
        if (!isMode(EditMode::Edit)) {
            return false;
        }
        if (inputEvent.isShortRelease(key_signs_remove)) {

            setCursorStartPosition(CursorStartPosition::Offset);

            if (!document->isEmpty() && removeChar()) {
                onTextChanged();
            }
            return true;
        }
        return false;
    }

    bool Text::handleWholeTextRemoval(const InputEvent &inputEvent)
    {
        if (!isMode(EditMode::Edit)) {
            return false;
        }
        if (inputEvent.isLongRelease(key_signs_remove)) {
            if (!document->isEmpty()) {
                clear();
                return true;
            }
        }
        return false;
    }

    bool Text::handleAddChar(const InputEvent &inputEvent)
    {
        if (!inputEvent.isShortRelease() || !isMode(EditMode::Edit)) {
            return false;
        }

        auto code = translator.handle(inputEvent.getRawKey(), mode ? mode->get() : "");

        if (code != Profile::none_key && checkAdditionBounds(code) == AdditionBound::CanAddAll) {

            setCursorStartPosition(CursorStartPosition::Offset);

            debug_text("handleAddChar %d -> Begin", code);
            debug_text("%s times: %" PRIu32, inputEvent.str().c_str(), translator.getTimes());
            /// if we have multi press in non digit mode - we need to replace char and put next char from translator
            if (!(mode->is(InputMode::digit) || (mode->is(InputMode::phone))) && translator.getTimes() > 0) {
                removeChar();
            }
            addChar(code);
            onTextChanged();

            debug_text("handleAddChar -> End(true)");

            return true;
        }

        debug_text("handleAdChar -> End(false)");
        return false;
    }

    auto Text::handleLongPressAddChar(const InputEvent &inputEvent) -> bool
    {
        if (!inputEvent.isLongRelease()) {
            return false;
        }
        if (!isMode(EditMode::Edit)) {
            return false;
        }

        // check input event handling accordingly to input mode
        auto code = text::npos;

        // phone mode
        if (mode->is(InputMode::phone) && inputEvent.is(KeyCode::KEY_0)) {
            code = '+';
        }
        // all other modes only handle digits
        else if (inputEvent.isDigit()) {
            code = intToAscii(inputEvent.numericValue());
        }

        if (code != text::npos && checkAdditionBounds(code) == AdditionBound::CanAddAll) {
            setCursorStartPosition(CursorStartPosition::Offset);
            addChar(code);
            onTextChanged();
            return true;
        }

        return false;
    }

    auto Text::makePreDrawLines(const uint32_t utfVal) -> std::unique_ptr<Lines>
    {
        auto preDrawLines = std::make_unique<Lines>(this);
        auto documentCopy = *cursor->getDocument();
        auto formatCopy   = getTextFormat().getFont();

        applyParentSizeRestrictions();

        BlockCursor preDrawCursor(&documentCopy, 0, 0, formatCopy);
        preDrawCursor.addChar(utfVal);

        preDrawLines->draw(
            preDrawCursor, getSizeMinusPadding(Axis::X, Area::Max), text::npos, padding.top, padding.left);

        return preDrawLines;
    }

    auto Text::makePreDrawLines(const TextBlock &textBlock) -> std::unique_ptr<Lines>
    {
        auto preDrawLines     = std::make_unique<Lines>(this);
        auto documentCopy     = *cursor->getDocument();
        auto formatCopy       = getTextFormat().getFont();
        auto preDrawTextBlock = TextBlock(textBlock);

        applyParentSizeRestrictions();

        BlockCursor preDrawCursor(&documentCopy, 0, 0, formatCopy);
        preDrawCursor.addTextBlock(std::move(preDrawTextBlock));

        preDrawLines->draw(
            preDrawCursor, getSizeMinusPadding(Axis::X, Area::Max), text::npos, padding.top, padding.left);

        return preDrawLines;
    }

    auto Text::checkMaxSignsLimit(unsigned int limitVal) -> AdditionBound
    {
        if (getText().length() >= limitVal) {
            debug_text("Text at max signs count can't add more");
            return AdditionBound::CantAdd;
        }
        else {
            return AdditionBound::CanAddAll;
        }
    }

    auto Text::checkMaxSignsLimit(const TextBlock &textBlock, unsigned int limitVal)
        -> std::tuple<AdditionBound, TextBlock>
    {
        if (getText().length() >= limitVal) {
            debug_text("Text at max signs count can't add more.");
            return {AdditionBound::CantAdd, textBlock};
        }
        else if (getText().length() + textBlock.length() >= limitVal) {

            // Split existing block into smaller one that can still fit and return it
            auto availableSpace = limitVal - getText().length();
            auto partBlockText  = textBlock.getText().substr(0, availableSpace);
            auto blockFormat    = textBlock.getFormat();

            debug_text("Text at max sign count adding part of block. Original: %s, Fit part %s",
                       textBlock.getText().c_str(),
                       partBlockText.c_str());

            return {AdditionBound::CanAddPart, TextBlock(partBlockText, std::make_unique<TextFormat>(*blockFormat))};
        }
        else {
            return {AdditionBound::CanAddAll, textBlock};
        }
    }

    auto Text::checkMaxSizeLimit(uint32_t utfVal) -> AdditionBound
    {
        auto returnValue = AdditionBound::CanAddAll;

        auto preDrawLines = makePreDrawLines(utfVal);

        debug_text("Text lines height: %d, preDraw height: %d, Widget max h: %d",
                   lines->linesHeight(),
                   preDrawLines->linesHeight(),
                   area(Area::Max).h);

        if (preDrawLines->maxWidth() == 0 ||
            preDrawLines->linesHeight() + getPadding().getSumInAxis(Axis::Y) > area(Area::Max).h) {

            debug_text("Text at max size can't add more");
            preDrawLines->erase();
            returnValue = AdditionBound::CantAdd;
        }

        preDrawLines->erase();
        return returnValue;
    }

    auto Text::checkMaxSizeLimit(const TextBlock &textBlock) -> std::tuple<AdditionBound, TextBlock>
    {
        auto preDrawLines = makePreDrawLines(textBlock);

        debug_text("Text lines height: %d, preDraw height: %d, Widget max h: %d",
                   lines->linesHeight(),
                   preDrawLines->linesHeight(),
                   area(Area::Max).h);

        if (preDrawLines->maxWidth() == 0) {
            preDrawLines->erase();
            return {AdditionBound::CantAdd, textBlock};
        }

        if (preDrawLines->linesHeight() + getPadding().getSumInAxis(Axis::Y) > area(Area::Max).h) {

            debug_text("Text at max size can't add whole bock, try to split");

            for (unsigned int signCounter = textBlock.length(); signCounter != 0; signCounter--) {

                preDrawLines->erase();

                auto partBlockText = textBlock.getText().substr(0, signCounter);
                auto blockFormat   = textBlock.getFormat();

                preDrawLines = makePreDrawLines(TextBlock(partBlockText, std::make_unique<TextFormat>(*blockFormat)));

                debug_text("Text lines height: %d, preDraw height: %d, Widget max h: %d",
                           lines->linesHeight(),
                           preDrawLines->linesHeight(),
                           area(Area::Max).h);

                if (preDrawLines->linesHeight() + getPadding().getSumInAxis(Axis::Y) <= area(Area::Max).h) {

                    preDrawLines->erase();

                    debug_text("Text at max size adding part of block. Original: %s, Fit part %s",
                               textBlock.getText().c_str(),
                               partBlockText.c_str());

                    return {AdditionBound::CanAddPart,
                            TextBlock(partBlockText, std::make_unique<TextFormat>(*blockFormat))};
                }
            }

            preDrawLines->erase();
            // If not a part of block can fit return hit bound.
            return {AdditionBound::CantAdd, textBlock};
        }

        preDrawLines->erase();
        return {AdditionBound::CanAddAll, textBlock};
    }

    auto Text::checkMaxLinesLimit(uint32_t utfVal, unsigned int limitVal) -> AdditionBound
    {
        auto returnValue = AdditionBound::CanAddAll;

        auto preDrawLines = makePreDrawLines(utfVal);

        debug_text("Text lines height: %d, preDraw height: %d, Widget max h: %d",
                   lines->linesHeight(),
                   preDrawLines->linesHeight(),
                   area(Area::Max).h);

        if (preDrawLines->size() > limitVal) {

            debug_text("Text at max size can't add more");
            preDrawLines->erase();
            returnValue = AdditionBound::CantAdd;
        }

        preDrawLines->erase();
        return returnValue;
    }

    auto Text::checkMaxLinesLimit(const TextBlock &textBlock, unsigned int limitVal)
        -> std::tuple<AdditionBound, TextBlock>
    {
        auto preDrawLines = makePreDrawLines(textBlock);

        debug_text("Text lines height: %d, preDraw height: %d, Widget max h: %d",
                   lines->linesHeight(),
                   preDrawLines->linesHeight(),
                   area(Area::Max).h);

        if (preDrawLines->maxWidth() == 0) {
            preDrawLines->erase();
            return {AdditionBound::CantAdd, textBlock};
        }

        if (preDrawLines->size() > limitVal) {

            debug_text("Text at max line size can't add whole bock, try to split");

            for (unsigned int signCounter = textBlock.length(); signCounter != 0; signCounter--) {

                preDrawLines->erase();

                auto partBlockText = textBlock.getText().substr(0, signCounter);
                auto blockFormat   = textBlock.getFormat();

                preDrawLines = makePreDrawLines(TextBlock(partBlockText, std::make_unique<TextFormat>(*blockFormat)));

                debug_text("Text lines height: %d, preDraw height: %d, Widget max h: %d",
                           lines->linesHeight(),
                           preDrawLines->linesHeight(),
                           area(Area::Max).h);

                if (preDrawLines->size() <= limitVal) {

                    preDrawLines->erase();

                    debug_text("Text at max line size adding part of block. Original: %s, Fit part %s",
                               textBlock.getText().c_str(),
                               partBlockText.c_str());

                    return {AdditionBound::CanAddPart,
                            TextBlock(partBlockText, std::make_unique<TextFormat>(*blockFormat))};
                }
            }

            preDrawLines->erase();
            // If not a part of block can fit return hit bound.
            return {AdditionBound::CantAdd, textBlock};
        }

        preDrawLines->erase();
        return {AdditionBound::CanAddAll, textBlock};
    }

    auto Text::checkAdditionBounds(const uint32_t utfVal) -> AdditionBound
    {
        auto returnValue = AdditionBound::CanAddAll;

        for (auto limit : limitsList) {

            switch (limit.limitType) {
            case TextLimitType::MaxSignsCount:
                returnValue = checkMaxSignsLimit(limit.limitValue);
                break;
            case TextLimitType::MaxLines:
                returnValue = checkMaxLinesLimit(utfVal, limit.limitValue);
                break;
            case TextLimitType::MaxSize:
                returnValue = checkMaxSizeLimit(utfVal);
                break;
            default:
                break;
            }

            if (returnValue == AdditionBound::CantAdd) {
                return returnValue;
            }
        }
        return returnValue;
    }

    auto Text::checkAdditionBounds(const TextBlock &textBlock) -> std::tuple<AdditionBound, TextBlock>
    {
        std::tuple<AdditionBound, TextBlock> returnValue = {AdditionBound::CanAddAll, textBlock};
        auto shortestProcessedBlock                      = textBlock;

        for (auto limit : limitsList) {

            switch (limit.limitType) {
            case TextLimitType::MaxSignsCount:
                returnValue = checkMaxSignsLimit(textBlock, limit.limitValue);
                break;
            case TextLimitType::MaxLines:
                returnValue = checkMaxLinesLimit(textBlock, limit.limitValue);
                break;
            case TextLimitType::MaxSize:
                returnValue = checkMaxSizeLimit(textBlock);
                break;
            default:
                break;
            }

            if (std::get<0>(returnValue) == AdditionBound::CanAddPart) {
                if (std::get<1>(returnValue).length() < shortestProcessedBlock.length()) {
                    shortestProcessedBlock = std::get<1>(returnValue);
                }
            }

            if (std::get<0>(returnValue) == AdditionBound::CantAdd) {
                return returnValue;
            }
        }

        return {std::get<0>(returnValue), shortestProcessedBlock};
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

    TextBackup Text::backupText() const
    {
        return TextBackup{std::list<TextBlock>(document->getBlocks().begin(), document->getBlocks().end()),
                          cursor->getAbsolutePosition()};
    }

    void Text::restoreFrom(const TextBackup &backup)
    {
        setText(std::make_unique<TextDocument>(backup.document));

        // If backup cursor position greater than new text length do not move cursor.
        if (getText().length() > backup.cursorPos) {
            auto cursorPosDiff = getText().length() - backup.cursorPos;

            // Move cursor to backup position from end of document.
            cursor->TextCursor::moveCursor(NavigationDirection::LEFT, cursorPosDiff);
        }
    }

    void Text::accept(GuiVisitor &visitor)
    {
        visitor.visit(*this);
    }
} /* namespace gui */
