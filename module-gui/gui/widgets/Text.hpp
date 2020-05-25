/*
 * @file Text.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 1 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_GUI_GUI_WIDGETS_TEXT_HPP_
#define MODULE_GUI_GUI_WIDGETS_TEXT_HPP_

#include <list>
#include <vector>

#include "utf8/UTF8.hpp"

#include "BoxLayout.hpp"
#include "Label.hpp"
#include "Rect.hpp"
#include "Style.hpp"
#include "InputMode.hpp"
#include "Translator.hpp"
#include "TextCursor.hpp"

namespace gui
{

    ///  @brief Widget that holds multiple lines of text.
    ///
    ///  Can expand horizontally to it's max size if it needs to fit more text in line
    ///  Can expand vertically if needed to hold lines of text.
    ///  Handles input
    ///        1. Provides text navigation
    ///        2. Provides new letter input with gui::KeyInputMappedTranslation
    ///        3. handles deletion of character
    ///
    ///   This widget stores both: whole text provided to it, and text visible on the screen
    ///   screen visible on the screen is a set of labels
    ///
    class Text : public Rect
    {
      public:
        enum class LineEndType
        {
            EOT,           // end of source text
            BREAK,         // line had enter defined as a /r
            CONTINUE,      // line was broken due to not enough space to hold all characters
            CONTINUE_SPACE // line was broken on the space character because next word doesn't fit current line.
        };

        /**
         * Types of expanding the text field
         */
        enum class ExpandMode
        {
            EXPAND_UP,
            EXPAND_DOWN,
            EXPAND_NONE // defult
        };

        /**
         * Modes of work of the text editor
         */
        enum class EditMode
        {
            BROWSE,
            EDIT,
            SCROLL,
        };

        /**
         * @brief Type of the text field
         */
        enum class TextType
        {
            SINGLE_LINE = 1,
            MULTI_LINE
        };

        enum class NavigationBarrier
        {
            BARRIER_NONE  = 0x00,
            BARRIER_UP    = 0x01,
            BARRIER_DOWN  = 0x02,
            BARRIER_LEFT  = 0x04,
            BARRIER_RIGHT = 0x08,
            BARRIER_ALL   = 0x0F,
        };

      protected:
        class TextLine
        {
          public:
            UTF8 text;
            uint32_t startIndex  = 0;
            uint32_t endIndex    = 0;
            LineEndType endType  = LineEndType::EOT;
            uint32_t pixelLength = 0;

            TextLine(
                const UTF8 &text, uint32_t startIndex, uint32_t endIndex, LineEndType endType, uint32_t pixelLength);
            /**
             * Returns text with appropriate ending.
             */
            UTF8 getText()
            {
                return text;
            }
            UTF8 getTextWithEnding()
            {
                UTF8 retText = text;
                if (endType == LineEndType::BREAK)
                    retText.insert("\n");
                else if (endType == LineEndType::CONTINUE_SPACE)
                    retText.insert(" ");
                return retText;
            }
        };

        // holds list of all lines that  text was divided to.
        std::list<TextLine *> textLines;
        // holds list of labels for displaying currently visible text lines.
        std::vector<gui::Label *> labelLines;
        // pointer to the first visible line of text
        std::list<TextLine *>::iterator firstLine = textLines.end();
        // pointer to the last visible line.
        std::list<TextLine *>::iterator lastLine = textLines.end();

        TextCursor *cursor = nullptr;

        EditMode editMode = EditMode::EDIT;

      public:
        ExpandMode expandMode = ExpandMode::EXPAND_NONE;

      private:
        TextType textType = TextType::MULTI_LINE;
        // maximum number of lines until which widget will expand its size;
        uint32_t maxExpansionLines = 0;
        // width of the cursor
        uint32_t cursorWidth = 2;
        // font pointer
        Font *font;
        // index of the first visible row of the text
        uint32_t firstRow = 0;
        // number of visible rows
        uint32_t visibleRows = 0;
        // text color
        Color textColor;
        // margins for text
        Margins margins;
        // barrier for navigation
        uint32_t barriers = static_cast<uint32_t>(NavigationBarrier::BARRIER_NONE);
        // default alignment
        Alignment alignment = style::text::defaultTextAlignment;
        // defines whether internal labels have underline
        bool underline = false;

        void splitTextToLines(const UTF8 &text);
        /**
         * Iterate over lines starting from the one that is provided. Function concatenate lines and performs new split.
         * Function stops on the last line or it there is a lines break ( enter ) or if last concatenated line doesn;t
         * change after update.
         */
        void reworkLines(std::list<TextLine *>::iterator it);
        /**
         * Splites text in two on ENTER, space or dot.
         * @return true if line was actually splitted
         */
        bool splitText(UTF8 &source, UTF8 &remaining, LineEndType &endType, uint32_t availableSpace);
        /**
         * Moves cursor in specified direction
         */
        bool moveCursor(const NavigationDirection &direction);
        /**
         * Function is used to move text up and down in the browsing mode
         */
        bool handleBrowsing(const InputEvent &inputEvent);
        /**
         * Function is used to move cursor in all directions when in the Wdit mode.
         */
        bool handleNavigation(const InputEvent &inputEvent);
        /**
         * Handles enter key
         */
        bool handleEnter();
        bool handleBackspace();

      public:
        /**
         * Inserts translated set of characters provided from key translation logics
         */
        bool handleChar(uint32_t chars);
        /**
         * Updates cursor position in the text widget
         */
        void updateCursor();

      private:
        /**
         * Returns text line where cursor is located
         */
        std::list<TextLine *>::iterator getCursorTextLine();
        /**
         * Returns horizontal pixel space available for text.
         */
        uint32_t getAvailableHPixelSpace()
        {
            int32_t space = widgetArea.w - (margins.getAlong(Axis::X) + innerMargins.getAlong(Axis::X));
            return (space < 0) ? 0 : space;
        }

        void recalculateDrawParams();
        /// check height available in h by rowCound and Font set
        /// if size needed is bigger and mode is set to Expand -> make item bigger
        /// TODO think about callback to parent on expansion
        int32_t expand(uint32_t rowCount, int32_t h);

      public:
        Text();
        Text(Item *parent,
             const uint32_t &x,
             const uint32_t &y,
             const uint32_t &w,
             const uint32_t &h,
             const UTF8 &text      = "",
             ExpandMode expandMode = ExpandMode::EXPAND_NONE,
             TextType textType     = TextType::MULTI_LINE);
        virtual ~Text();

        /**
         * @brief allows changing behavior of the text widget between edit and browse mode.
         */
        void setEditMode(EditMode mode);
        void setTextType(TextType type);
        /**
         * @brief Sets mode where all text lines have underline.
         * @param underline - if true text lines will have underline, if false no underline will be drawn.
         */
        void setUnderline(bool underline);
        void setNavigationBarrier(const NavigationBarrier &barrier, bool value);
        void setCursorWidth(uint32_t w);
        virtual void setText(const UTF8 &text);
        virtual void clear();
        virtual UTF8 getText();
        /**
         * @brief Saves text from widget to file at specified path
         * @param path Path to output file where widgets content will be saved.
         * @return True if operation was successful, false otherwise.
         */
        virtual bool saveText(UTF8 path);
        void setFont(const UTF8 &fontName);
        virtual void setMargins(const Margins &margins);

        // virtual methods from Item
        std::list<DrawCommand *> buildDrawList() override;
        void setPosition(const short &x, const short &y) override;
        void setSize(const unsigned short w, const unsigned short h) override;
        bool onInput(const InputEvent &inputEvent) override;
        /// move ownership of mode ptr to Text
        void setInputMode(InputMode *&&mode)
        {
            this->mode = mode;
        };
        bool onActivated(void *data) override;
        bool onFocus(bool state) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
        Item *getNavigationItem(NavigationDirection direction) override;
        void setRadius(int value) override;
        bool onContent() override;
        void setTextColor(Color color);
        void setAlignment(const Alignment alignment);
        void setYaps(RectangleYapFlags yaps) override;

      private:
        InputMode *mode = nullptr;
        gui::KeyInputMappedTranslation translator;
    };

} /* namespace gui */

#endif /* MODULE_GUI_GUI_WIDGETS_TEXT_HPP_ */
