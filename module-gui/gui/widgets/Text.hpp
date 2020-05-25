#pragma once

#include <vector>

#include "utf8/UTF8.hpp"

#include "BoxLayout.hpp"
#include "Label.hpp"
#include "Rect.hpp"
#include "Style.hpp"
#include "InputMode.hpp"
#include "Translator.hpp"
#include "TextCursor.hpp"
#include "TextDocument.hpp"

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
      protected:
        // holds list of labels for displaying currently visible text lines.
        std::vector<gui::Label *> labelLines;

        TextCursor *cursor = nullptr;
        std::unique_ptr<TextDocument> document = std::make_unique<TextDocument>();

        EditMode editMode = EditMode::EDIT;

      public:
        ExpandMode expandMode = ExpandMode::EXPAND_NONE;

      private:
        TextType textType = TextType::MULTI_LINE;
        // maximum number of lines until which widget will expand its size;
        uint32_t maxExpansionLines = 0;
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
        bool moveCursor(const NavigationDirection &direction, std::unique_ptr<TextDocument> &document);
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
