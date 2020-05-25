/*
 * @file Text.cpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#include <iterator>

#include "../core/Font.hpp"
#include "Ellipsis.hpp"
#include "Text.hpp"
#include "log/log.hpp"
#include "utf8/UTF8.hpp"
#include "vfs.hpp"
#include <Style.hpp>
#include <cassert>

namespace gui
{

    Text::TextLine::TextLine(
        const UTF8 &text, uint32_t startIndex, uint32_t endIndex, Text::LineEndType endType, uint32_t pixelLength)
        : text{text}, startIndex{startIndex}, endIndex{endIndex}, endType{endType}, pixelLength{pixelLength}
    {}

    Text::Text(Item *parent,
               const uint32_t &x,
               const uint32_t &y,
               const uint32_t &w,
               const uint32_t &h,
               const UTF8 &text,
               ExpandMode expandMode,
               TextType textType)
        : Rect(parent, x, y, w, h), expandMode{expandMode}, textType{textType}
    {

        uint32_t fontID = FontManager::getInstance().getFontID(style::window::font::small);
        font            = FontManager::getInstance().getFont(fontID);
        cursor          = new TextCursor(this);

        if (text.length() != 0u) {
            splitTextToLines(text);
        }
        else {
            textLines.push_back(new TextLine(UTF8(""), 0, 0, LineEndType::EOT, 0));
            firstLine = textLines.begin();
            lastLine  = textLines.begin();
        }
        setBorderColor(gui::ColorFullBlack);
        setEdges(RectangleEdgeFlags::GUI_RECT_ALL_EDGES);
        // TODO this is bad - cursor->column is badly handled on newline etc.
        cursor->column += text.length();
        updateCursor();
    }

    Text::Text() : Text(nullptr, 0, 0, 0, 0)
    {}

    Text::~Text()
    {
        // if there are text lines erase them.
        if (!textLines.empty()) {
            while (!textLines.empty()) {
                delete textLines.front();
                textLines.pop_front();
            }
        }
        if (mode) {
            delete mode;
        }
    }

    void Text::setYaps(RectangleYapFlags yaps)
    {
        Rect::setYaps(yaps);
        recalculateDrawParams();
    }

    void Text::setEditMode(EditMode mode)
    {
        editMode = mode;
        cursor->setVisible(mode != EditMode::BROWSE && focus);
    }

    void Text::setTextType(TextType type)
    {
        textType = type;
    }

    void Text::setUnderline(bool underline)
    {
        // do nothing, value of the flag doesn;t change
        if (this->underline == underline)
            return;

        this->underline = underline;
        // LOG_INFO("lines count: %d", labelLines.size());
        for (auto it = labelLines.begin(); it != labelLines.end(); it++) {

            gui::Label *label = *it;
            if (this->underline)
                label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
            else
                label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        }
    }

    void Text::setNavigationBarrier(const NavigationBarrier &barrier, bool value)
    {
        if (value)
            barriers |= static_cast<uint32_t>(barrier);
        else
            barriers &= ~(static_cast<uint32_t>(barrier));
    }

    void Text::setCursorWidth(uint32_t w)
    {
        cursorWidth = w;
    }

    void Text::setText(const UTF8 &text)
    {
        clear();
        cursor->reset();
        if (text.length() > 0) {
            // erase default empty line
            delete textLines.front();
            textLines.pop_front();
            textLines.clear();
            // split and add new lines
            splitTextToLines(text);
        }
        recalculateDrawParams();
        updateCursor();
    }

    void Text::setTextColor(Color color)
    {
        textColor = color;
        for (auto line : labelLines) {
            line->setTextColor(color);
        }
    }

    void Text::clear()
    {
        // if there are text lines erase them.
        if (!textLines.empty()) {
            while (!textLines.empty()) {
                delete textLines.front();
                textLines.pop_front();
            }
        }
        textLines.clear();
        // insert first empty text line
        textLines.push_back(new TextLine(UTF8(""), 0, 0, LineEndType::EOT, 0));
        firstLine = textLines.begin();
        lastLine  = textLines.begin();
    }

    UTF8 Text::getText()
    {

        UTF8 output;

        // iterate over all lines and add content from each line to output string.
        auto it = textLines.begin();
        while (it != textLines.end()) {

            auto textLine = (*it);
            assert(textLine);
            output += textLine->text;
            if (textLine->endType == LineEndType::BREAK) {
                output.insert("\n");
            }

            if (textLine->endType == LineEndType::CONTINUE_SPACE) {
                output.insert(" ");
            }

            ++it;
        }
        return output;
    }

    bool Text::saveText(UTF8 path)
    {

        auto file = vfs.fopen(path.c_str(), "wb");

        if (file == nullptr)
            return false;

        auto it = textLines.begin();

        // iterate over all lines in text edit
        while (it != textLines.end()) {

            vfs.fwrite((*it)->text.c_str(), (*it)->text.used() - 1, 1, file);
            if ((*it)->endType == LineEndType::BREAK) {
                vfs.fwrite("\n", 1, 1, file);
            }

            if ((*it)->endType == LineEndType::CONTINUE_SPACE) {
                vfs.fwrite(" ", 1, 1, file);
            }

            ++it;
        }
        // close file
        vfs.fclose(file);

        return true;
    }

    void Text::setFont(const UTF8 &fontName)
    {

        uint32_t fontID = FontManager::getInstance().getFontID(fontName.c_str());
        Font *newFont   = FontManager::getInstance().getFont(fontID);
        if (newFont != nullptr) {
            font = newFont;
            //		calculateDisplayText();
        }
        else {
            LOG_ERROR("Font not found");
        }
        recalculateDrawParams();
    }

    void Text::splitTextToLines(const UTF8 &text)
    {

        if (text.length() == 0) {
            if (textLines.size() == 0) {
                firstLine = textLines.end();
                lastLine  = textLines.end();
            }
            return;
        }

        // copy provided text to internal buffer
        uint32_t index       = 0;
        uint32_t totalLength = text.length();

        uint32_t availableSpace = getAvailableHPixelSpace();

        while (index < totalLength) {

            UTF8 textCopy = text.substr(index, totalLength - index);
            uint32_t charCount     = font->getCharCountInSpace(textCopy, availableSpace);
            UTF8 tmpText           = textCopy.substr(0, charCount);
            uint32_t spaceConsumed = font->getPixelWidth(tmpText);

            // some default values
            uint32_t startIndex     = 0;
            uint32_t endIndex       = totalLength;
            LineEndType lineEndType = LineEndType::EOT;

            // check if this is not the end of the text
            if (index + charCount == totalLength) {
                // try to find first enter.
                auto enterIndex = tmpText.find("\n", 0);
                if (enterIndex != UTF8::npos) {
                    endIndex = index + enterIndex;
                    index += enterIndex + 1;
                    lineEndType = LineEndType::BREAK;
                    textLines.push_back(new TextLine(tmpText.substr(0, enterIndex),
                                                     startIndex,
                                                     endIndex,
                                                     lineEndType,
                                                     font->getPixelWidth(tmpText.substr(0, enterIndex))));
                    //				LOG_INFO("Text Input Line: [%s]", textLines.back()->text.c_str());
                } // no enter found last line can be copied as a whole.
                else {
                    startIndex = index;
                    endIndex   = totalLength;
                    textLines.push_back(new TextLine(tmpText, startIndex, endIndex, lineEndType, spaceConsumed));
                    //				LOG_INFO("Text Input Line: [%s]", textLines.back()->text.c_str());
                    index += charCount;
                }
            }
            // if it wasn't the last line search for enter or space and break the line on it.
            else {

                startIndex = index;

                // try to find first enter.
                auto enterIndex = tmpText.find("\n", 0);
                if (enterIndex != UTF8::npos) {
                    endIndex = index + enterIndex;
                    index += enterIndex + 1;
                    lineEndType = LineEndType::BREAK;
                    textLines.push_back(
                        new TextLine(tmpText.substr(0, enterIndex), startIndex, endIndex, lineEndType, spaceConsumed));
                    //				LOG_INFO("Text Input Line: [%s]", textLines.back()->text.c_str());
                }
                else {
                    // if there was no enter look for last space in the tmpText and break line on it
                    auto spaceIndex = tmpText.findLast(" ", tmpText.length() - 1);

                    // if there was no space take as many characters as possible and add CONTINUE ending
                    if (spaceIndex == UTF8::npos) {
                        endIndex = index + charCount;
                        index += charCount;
                        lineEndType = LineEndType::CONTINUE;
                        textLines.push_back(new TextLine(tmpText, startIndex, endIndex, lineEndType, spaceConsumed));
                        //					LOG_INFO("Text Input Line: [%s]", textLines.back()->text.c_str());
                    }
                    else {
                        lineEndType = LineEndType::CONTINUE_SPACE;

                        uint32_t spaceWidth = font->getPixelWidth(" ", 0, 1);
                        // if space is last character in string erase it and add appropriate CONTINUE_SPACE ending
                        if (spaceIndex == tmpText.length() - 1) {
                            endIndex = index + charCount - 1;
                            index += charCount;
                            textLines.push_back(new TextLine(tmpText.substr(0, tmpText.length() - 1),
                                                             startIndex,
                                                             endIndex,
                                                             lineEndType,
                                                             spaceConsumed - spaceWidth));
                            //						LOG_INFO("Text Input Line: [%s]", textLines.back()->text.c_str());
                        }
                        else {
                            endIndex = index + spaceIndex;
                            index += spaceIndex + 1;
                            textLines.push_back(new TextLine(tmpText.substr(0, spaceIndex),
                                                             startIndex,
                                                             endIndex,
                                                             lineEndType,
                                                             font->getPixelWidth(tmpText.substr(0, spaceIndex))));
                            //						LOG_INFO("Text Input Line: [%s]", textLines.back()->text.c_str());
                        }
                    }
                }
            }

            if (textType == TextType::SINGLE_LINE) {
                // LOG_INFO("NUMBER OF LINES: %d", textLines.size());
                auto textLine     = textLines.front();
                textLine->endType = LineEndType::EOT;
                break;
            }
        }

        firstLine = textLines.begin();
        lastLine  = textLines.begin();
    }

    bool Text::splitText(UTF8 &source, UTF8 &remaining, LineEndType &endType, uint32_t availableSpace)
    {

        uint32_t charCount = font->getCharCountInSpace(source, availableSpace);
        UTF8 searchStr = source.substr(0, charCount);

        // try to find first enter.
        uint32_t enterIndex = searchStr.find("\n", 0);
        if (enterIndex != UTF8::npos) {
            endType   = LineEndType::BREAK;
            remaining = source.substr(enterIndex, source.length() - 1 - enterIndex);
            source.split(enterIndex);
            //		LOG_INFO("Split Text: source: [%s] remaining: [%s]", source.c_str(), remaining.c_str());
            return true;
        }
        else {
            // if there was no enter look for last space in the source and break line on it
            uint32_t spaceIndex = searchStr.findLast(" ", searchStr.length() - 1);

            // if there was no space take as many characters as possible and add CONTINUE ending
            if (spaceIndex == UTF8::npos) {
                remaining = source.split(charCount);
                endType   = LineEndType::CONTINUE;
                //			LOG_INFO("Split Text: source: [%s] remaining: [%s]", source.c_str(), remaining.c_str());
                return true;
            }
            else {
                endType = LineEndType::CONTINUE_SPACE;

                remaining = source.substr(spaceIndex + 1, source.length() - 1 - spaceIndex);
                source.split(spaceIndex);
                //			LOG_INFO("Split Text: source: [%s] remaining: [%s]", source.c_str(), remaining.c_str());
                return true;
            }
        }
        return false;
    }

    void Text::reworkLines(std::list<TextLine *>::iterator it)
    {

        // iterate until end of text lines or till line that fits available space has break line ending (enter).
        while (it != textLines.end()) {

            // if current line has BREAK of EOT line ending check if current text fits available space
            // finish procedure
            uint32_t availableSpace = getAvailableHPixelSpace();
            uint32_t consumedSpace;

            if (((*it)->endType == LineEndType::BREAK) || ((*it)->endType == LineEndType::EOT)) {
                consumedSpace = font->getPixelWidth((*it)->getText());
                if (consumedSpace < availableSpace)
                    break;
            }

            // check if there is next line
            auto itNext = it;
            itNext++;

            UTF8 mergedLinesText = (*it)->getTextWithEnding();

            // if processed text line is not finished with break end type
            if (((*it)->endType != LineEndType::BREAK) && (itNext != textLines.end())) {

                // merge text from two lines
                mergedLinesText += (*itNext)->getTextWithEnding();
                // assign end type from next line to the current line
                (*it)->endType = (*itNext)->endType;

                // remove next line as the text was taken to the current line
                delete (*itNext);
                textLines.erase(itNext);
            }

            LineEndType endType;
            UTF8 remainingText;
            bool splitFlag = splitText(mergedLinesText, remainingText, endType, availableSpace);

            // if there was a split update current and next item in the list
            if (splitFlag) {

                (*it)->text        = std::move(mergedLinesText);
                (*it)->pixelLength = font->getPixelWidth((*it)->getText());

                itNext = it;
                itNext++;
                textLines.insert(
                    itNext,
                    new TextLine(
                        remainingText, 0, remainingText.length(), (*it)->endType, font->getPixelWidth(remainingText)));

                (*it)->endType = endType;
            }

            // proceed to next line
            it++;
        }

        // TODO starting from first modified line up to last modified line update start and end index
    }

    std::list<DrawCommand *> Text::buildDrawList()
    {
        return Rect::buildDrawList();
    }
    void Text::setPosition(const short &x, const short &y)
    {
        Rect::setPosition(x, y);
        recalculateDrawParams();
        updateCursor();
    }

    void Text::setSize(const unsigned short w, const unsigned short h)
    {
        Rect::setSize(w, h);
        recalculateDrawParams();
        updateCursor();
    }

    bool Text::onInput(const InputEvent &inputEvent)
    {
        bool res = false;

        if (inputCallback && inputCallback(*this, inputEvent)) {
            return true;
        }

        if (inputEvent.state == InputEvent::State::keyReleasedLong && inputEvent.keyCode == gui::KeyCode::KEY_AST) {
            if (mode) {
                mode->select_special_char();
                return true;
            }
        }

        if (inputEvent.state == InputEvent::State::keyReleasedShort && inputEvent.keyCode == KeyCode::KEY_ENTER) {
            if (activatedCallback && activatedCallback(*this)) {
                return true;
            }
        }

        // handle navigation
        if (inputEvent.state == InputEvent::State::keyReleasedShort &&
            ((inputEvent.keyCode == KeyCode::KEY_LEFT) || (inputEvent.keyCode == KeyCode::KEY_RIGHT) ||
             (inputEvent.keyCode == KeyCode::KEY_UP) || (inputEvent.keyCode == KeyCode::KEY_DOWN))) {
            switch (editMode) {
            case EditMode::BROWSE:
                res = handleBrowsing(inputEvent);
                break;
            case EditMode::EDIT:
                res = handleNavigation(inputEvent);
                break;
            case EditMode::SCROLL:
                LOG_DEBUG("TODO");
                res = false;
                break;
            }

            if (res && editMode != EditMode::SCROLL)
                updateCursor();
            return res;
        }

        // translate and store keypress
        uint32_t code = translator.handle(inputEvent.key, mode ? mode->get() : "");

        /// handling of key removal
        if (inputEvent.keyCode == gui::KeyCode::KEY_PND) {
            // longpress -> remove all characters
            if (inputEvent.state == InputEvent::State::keyReleasedLong) {
                // TODO handle longpress remove key
            }
            // shortpress remove characters n time for multipress, one time for normal press
            else if (inputEvent.state == InputEvent::State::keyReleasedShort) {
                auto char_rm = [=](bool &res) {
                    res = handleBackspace();
                    if (res) {
                        updateCursor();
                        contentCallback(*this);
                    }
                };
                if (translator.getTimes()) {
                    for (unsigned int i = 0; i < translator.getTimes(); ++i) {
                        char_rm(res);
                    }
                }
                else {
                    char_rm(res);
                }
                return res;
            }
        }

        // process only short release events
        if (inputEvent.state != InputEvent::State::keyReleasedShort) {
            return false;
        }

        if (inputEvent.keyCode == gui::KeyCode::KEY_AST) {
            if (mode) {
                mode->next();
                return true;
            }
        }

        // it there is no key char it means that translator didn't handled the key and this key
        if (code == 0) {
            LOG_DEBUG("Key not handled! %d", static_cast<int>(inputEvent.keyCode));
            return false;
        }

        // get how many short presses were handled in this widget
        if (translator.getTimes()) {
            handleBackspace();
            res = handleChar(code);
            if (res) {
                updateCursor();
                contentCallback(*this);
            }
            return res;
        }

        // if char is a new line char then create new line and move caret and return
        if (code == 0x0A) {
            if (textType == TextType::MULTI_LINE) {
                res = handleEnter();
                contentCallback(*this);
            }
        }
        else { // normal char -> add and check pixel width
            res = handleChar(code);
            contentCallback(*this);
        }
        if (res)
            updateCursor();
        return res;
    }

    bool Text::onActivated(void *data)
    {
        Rect::onActivated(data);
        return false;
    }

    bool Text::onFocus(bool state)
    {
        // inform on start what type of input there is
        if (mode) {
            mode->on_focus(state);
        }
        bool ret = Rect::onFocus(state);
        if (focus && editMode == EditMode::EDIT) {
            cursor->setVisible(visible);
            for (auto it = labelLines.begin(); it != labelLines.end(); it++)
                (*it)->setPenWidth(3);
        }
        else {
            cursor->setVisible(false);
            for (auto it = labelLines.begin(); it != labelLines.end(); it++)
                (*it)->setPenWidth(1);
        }

        return ret;
    }

    void Text::setRadius(int value)
    {
        Rect::setRadius(value);
        // if margins are smaller than radius update the margins
        if (margins.left < value)
            margins.left = value;
        if (margins.right < value)
            margins.right = value;
        updateCursor();
    }

    bool Text::onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim)
    {
        Rect::onDimensionChanged(oldDim, newDim);
        updateCursor();
        return false;
    }

    bool Text::moveCursor(const NavigationDirection &direction)
    {

        auto it = getCursorTextLine();

        if (direction == NavigationDirection::LEFT) {
            // if we are standing on the beginning for the line move to previous line
            if (cursor->column == 0) {

                // if there is no previous line return false so window can switch focus to the item on the left.
                if (it == textLines.begin()) {
                    return false;
                }

                // there is a previous line, check if cursor's row is greater than 0;
                cursor->column = (*std::prev(it, 1))->text.length();
                if (cursor->row > 0) {
                    --cursor->row;
                }
                else {
                    --firstLine;
                    recalculateDrawParams();
                }
                return true;
            }
            // cursor's column is greater than 0
            else {
                --cursor->column;
                return true;
            }
        }
        else if (direction == NavigationDirection::RIGHT) {
            // if cursor is not at the end of current line simply move curosr
            if (cursor->column < (*it)->text.length()) {
                ++cursor->column;
                return true;
            }
            else {
                auto itNext = std::next(it, 1);
                // if this is not the last line increment row and set column to 0
                if (itNext != textLines.end()) {
                    ++cursor->row;
                    cursor->column = 0;

                    // if increased row is out of visible are increment first line
                    if (cursor->row >= visibleRows) {
                        firstLine++;
                        recalculateDrawParams();
                        cursor->row = visibleRows - 1;
                    }
                    return true;
                }
            }
        }
        else if (direction == NavigationDirection::DOWN) {

            // if this is a single line text widget there is no navigation down allowed
            if (textType == TextType::SINGLE_LINE)
                return false;

            auto itNext = std::next(it, 1);

            // this is the last line, check for barrier
            if (itNext == textLines.end()) {
                if (barriers & static_cast<uint32_t>(NavigationBarrier::BARRIER_DOWN))
                    return true;
                return false;
            }

            // increment line
            ++cursor->row;

            // check if column position is still valid
            if (cursor->column > (*itNext)->text.length())
                cursor->column = (*itNext)->text.length();

            if (cursor->row >= visibleRows) {
                firstLine++;
                recalculateDrawParams();
                cursor->row = visibleRows - 1;
            }
            return true;
        }
        else if (direction == NavigationDirection::UP) {

            // if this is a single line text widget there is no navigation up allowed
            if (textType == TextType::SINGLE_LINE)
                return false;

            // if cursor is standing on the first line return false to allow focus change to previous widget
            if (it == textLines.begin()) {
                return false;
            }

            auto itPrev = std::prev(it, 1);
            if (cursor->row == 0) {
                --firstLine;

                recalculateDrawParams();
                return true;
            }
            else {
                --cursor->row;
            }

            // check if previous line is shorter than last one if so move cursor to the end of previous line
            if (cursor->column > (*itPrev)->text.length()) {
                cursor->column = (*itPrev)->text.length();
            }

            return true;
        }

        return false;
    }

    bool Text::handleBrowsing(const InputEvent &inputEvent)
    {
        // if this is a single line text widget there is no browsing allowed
        if (textType == TextType::SINGLE_LINE)
            return false;

        switch (inputEvent.keyCode) {
        case (KeyCode::KEY_UP): {
            // move cursor to first visible element
            cursor->row = 0;
            return moveCursor(NavigationDirection::UP);
        } break;
        case KeyCode::KEY_DOWN: {
            // move cursor to the last visible element
            auto it   = firstLine;
            cursor->row = 0;
            while ((it != textLines.end()) && (cursor->row < visibleRows - 1)) {
                it++;
                cursor->row++;
            }

            return moveCursor(NavigationDirection::DOWN);
        } break;
        default: {
            LOG_ERROR("Received unknown navigation key");
        }
        };
        return false;
    }

    bool Text::handleNavigation(const InputEvent &inputEvent)
    {

        switch (inputEvent.keyCode) {
        case (KeyCode::KEY_UP): {
            return moveCursor(NavigationDirection::UP);
        } break;
        case KeyCode::KEY_DOWN: {
            return moveCursor(NavigationDirection::DOWN);
        } break;
        case KeyCode::KEY_LEFT: {
            return moveCursor(NavigationDirection::LEFT);
        } break;
        case KeyCode::KEY_RIGHT: {
            return moveCursor(NavigationDirection::RIGHT);
        } break;
        default: {
            LOG_ERROR("Received unknown navigation key");
        }
        };
        return false;
    }

    bool Text::handleEnter()
    {

        if (editMode == EditMode::BROWSE)
            return false;

        // get textline where cursor is located
        auto it = firstLine;
        std::advance(it, cursor->row);

        // split current text in line using cursors position
        UTF8 remainingText = (*it)->text.split(cursor->column);

        // store old type of line ending set new type of ending to the current line
        LineEndType endType = (*it)->endType;
        (*it)->endType      = LineEndType::BREAK;

        // create and add new line using remaining parts of text
        auto itNext = it;
        ++itNext;
        textLines.insert(
            itNext,
            new TextLine(remainingText, 0, remainingText.length(), endType, font->getPixelWidth(remainingText)));
        cursor->row++;

        if (cursor->row >= visibleRows) {
            cursor->row = visibleRows - 1;
            firstLine++;
        }

        cursor->column = 0;

        reworkLines(it);

        recalculateDrawParams();

        return true;
    }

    bool Text::handleBackspace()
    {

        if (editMode == EditMode::BROWSE)
            return false;

        // if cursor is in column 0 and there is no previous line return
        if (cursor->atTextBegin())
            return true;

        // if cursor is in position other than 0 remove previous character and run lines rework
        auto it = getCursorTextLine();
        if (cursor->column > 0) {
            TextLine *currentTextLine = (*it);
            currentTextLine->text.removeChar(cursor->column - 1);
            cursor->column--;
        }
        // this is when cursor is located at the beginning of the line and there are previous lines
        else {

            if (it == textLines.begin()) {
                return true;
            }

            auto itPrev = std::prev(it, 1);

            // if ending is equal to LineEndType::CONTINUE delete last char from current string
            if ((*itPrev)->endType == LineEndType::CONTINUE) {
                (*itPrev)->text.removeChar((*itPrev)->text.length() - 1);
            }
            if ((*itPrev)->endType == LineEndType::CONTINUE_SPACE) {
                (*itPrev)->endType = LineEndType::CONTINUE;
            }
            else if ((*itPrev)->endType == LineEndType::BREAK) {
                (*itPrev)->endType = LineEndType::CONTINUE;
            }

            cursor->column = (*itPrev)->text.length();

            if (cursor->row == 0) {
                firstLine = itPrev;
            }
            else {
                --cursor->row;
            }

            (*itPrev)->text += (*it)->text;
            (*itPrev)->endType = (*it)->endType;

            // delete current line
            textLines.erase(it);
            it = itPrev;
        }

        reworkLines(it);
        recalculateDrawParams();

        return true;
    }

    bool Text::handleChar(uint32_t chars)
    {

        if (editMode == EditMode::BROWSE)
            return false;

        // get text line where cursor is standing
        auto it                   = getCursorTextLine();
        TextLine *currentTextLine = (*it);

        // TODO
        // calculate width of the character that is going to be inserted
        uint32_t charWidth = font->getCharPixelWidth(chars);
        if (charWidth == 0) {
            return false;
        }

        // insert character into string in currently selected line
        if (currentTextLine->text.insertCode(chars, cursor->column) == false)
            return false;

        // if sum of the old string and width of the new character are greater than available space run lines rework
        // procedure
        uint32_t availableSpace = getAvailableHPixelSpace();
        uint32_t currentWidth   = currentTextLine->pixelLength;
        if (currentWidth + charWidth > availableSpace) {

            // this is the case when new character inserted into single line text
            // is creating the line that doesn't fit available space.
            if (textType == TextType::SINGLE_LINE) {

                currentTextLine->text.removeChar(cursor->column, 1);
                return true;
            }

            ++cursor->column;
            reworkLines(it);

            // if cursor position is greater than number of characters in current line move cursor to next line.
            if (cursor->column > (*it)->text.length()) {
                cursor->column = 0;
                ++cursor->row;
            }

            if (cursor->row >= visibleRows) {
                firstRow++;
            }
        }
        // no line splitting, update pixel width and proceed
        else {
            currentTextLine->pixelLength = font->getPixelWidth(currentTextLine->text);
            ++cursor->column;
        }

        recalculateDrawParams();
        // calculate new position of the cursor

        return true;
    }

    std::list<Text::TextLine *>::iterator Text::getCursorTextLine()
    {
        auto it = firstLine;
        // TODO add check for distance to advance
        std::advance(it, cursor->row);
        return it;
    }

    void Text::updateCursor()
    {
        cursor->setSize(2, font->info.line_height);
        auto it = std::next(firstLine, cursor->row);

        uint32_t posX = margins.left + innerMargins.left + font->getPixelWidth((*it)->text, 0, cursor->column);
        uint32_t posY = (margins.top + innerMargins.top) + cursor->row * font->info.line_height;
        cursor->setPosition(posX, posY);
    }

    int32_t Text::expand(uint32_t rowCount, int32_t h)
    {
        if (rowCount < textLines.size() && expandMode != Text::ExpandMode::EXPAND_NONE) {
            h = font->info.line_height * textLines.size() +
                (margins.getAlong(Axis::Y) + innerMargins.getAlong(Axis::Y));
            if (parent && widgetArea.h > parent->widgetArea.h) {
                h = widgetArea.h;
            }
            setSize(getWidth(), h);
            LOG_DEBUG("Resized Text to: %" PRIu32 " %" PRIu32, getWidth(), getHeight());
        }
        return h;
    }

    void Text::recalculateDrawParams()
    {

        // calculate number of lines for displaying text
        int32_t h = widgetArea.h - (margins.getAlong(Axis::Y) + innerMargins.getAlong(Axis::Y));
        if (h < 0)
            h = 0;

        // remove all old labels
        for (uint32_t i = 0; i < labelLines.size(); ++i) {
            removeWidget(labelLines[i]);
            delete labelLines[i];
        }

        labelLines.clear();

        // calculate how many rows can fit in available height.
        uint32_t rowCount = h / font->info.line_height;
        h                 = expand(rowCount, h);

        if (textType == TextType::SINGLE_LINE) {
            rowCount = 1;
        }

        // if there is not enough space for single line start from 0 and ignore vertical margins
        uint16_t startY = (h < font->info.line_height) ? 0 : (margins.top + innerMargins.top);

        // create labels to display text. There will be always at least one.
        for (uint32_t i = 0; i < rowCount; i++) {
            gui::Label *label =
                new gui::Label(this,
                               (margins.left + innerMargins.left),
                               startY,
                               widgetArea.w - (margins.getAlong(Axis::X) + innerMargins.getAlong(Axis::X)),
                               font->info.line_height);
            label->setFilled(false);
            label->setPenWidth(1);
            label->setPenFocusWidth(3);
            label->setAlignment(alignment);
            label->setFont(font->getName());
            label->setTextColor(textColor);
            label->setEllipsis(gui::Ellipsis::None);

            if (underline)
                label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);
            else
                label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
            if (focus)
                label->setPenWidth(3);
            labelLines.push_back(label);
            startY += font->info.line_height;
        }
        visibleRows = rowCount;

        // assign text to all lines
        auto textIterator = firstLine;
        /// if there is less lines than possible to show, copy only needed lines
        auto num_lines_visible = labelLines.size() > textLines.size() ? textLines.size() : labelLines.size();
        for (uint32_t i = 0; i < num_lines_visible; i++) {
            if (textIterator == textLines.end())
                break;
            labelLines[i]->setText((*textIterator)->text);
            lastLine = textIterator;
            textIterator++;
        }
    }

    void Text::setMargins(const Margins &margins)
    {
        this->margins = margins;
        recalculateDrawParams();
        updateCursor();
    }

    Item *Text::getNavigationItem(NavigationDirection direction)
    {
        // if provided direction is forbidden than return nullptr
        if (barriers & static_cast<uint32_t>(direction))
            return nullptr;

        // otherwise run default navigation method (Item)
        return Rect::getNavigationItem(direction);
    }

    bool Text::onContent()
    {
        if ((expandMode == Text::ExpandMode::EXPAND_DOWN) || (expandMode == Text::ExpandMode::EXPAND_UP)) {
            if ((parent->type == ItemType::VBOX) || (parent->type == ItemType::HBOX)) {
                parent->onContent();
            }
        }

        return true;
    }

    void Text::setAlignment(const Alignment _alignment)
    {
        alignment = _alignment;
        recalculateDrawParams();
        updateCursor();
    }

} /* namespace gui */
