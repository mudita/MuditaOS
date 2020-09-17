#pragma once

#include <memory>
#include <vector>

#include "Alignment.hpp"
#include "InputEvent.hpp"
#include "TextConstants.hpp"
#include "utf8/UTF8.hpp"

#include "BoxLayout.hpp"
#include "InputMode.hpp"
#include "Label.hpp"
#include "Rect.hpp"
#include "Style.hpp"
#include "TextCursor.hpp"
#include "TextDocument.hpp"
#include "TextLine.hpp"
#include "Translator.hpp"

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
        friend TextCursor;

      protected:
        // holds list of labels for displaying currently visible text lines.
        class Lines
        {
            Item *parent = nullptr;
            std::list<TextLine> lines;

          public:
            void erase()
            {
                if (parent != nullptr) {
                    for (auto &line : lines) {
                        line.erase();
                    }
                }
                lines.clear();
            }

            void emplace(TextLine &&line)
            {
                lines.emplace_back(std::move(line));
            }

            Lines(Item *parent)
            {
                this->parent = parent;
            }

            const auto &get()
            {
                return lines;
            }

            auto &last()
            {
                return lines.back();
            }

            auto size()
            {
                return lines.size();
            }

            auto maxWidth()
            {
                unsigned int w = 0;
                // could be std::max_element
                for (auto &el : lines) {
                    w = el.width() > w ? el.width() : w;
                }
                return w;
            }

            auto linesHeight()
            {
                unsigned int h = 0;
                for (auto &el : lines) {
                    h += el.height();
                }
                return h;
            }

            auto linesVAlign(Length parentSize)
            {
                for (auto &line : lines) {
                    line.alignV(parent->getAlignment(Axis::Y), parentSize, linesHeight());
                }
            }

            auto linesHAlign(Length parentSize)
            {
                for (auto &line : lines) {
                    line.alignH(parent->getAlignment(Axis::X), parentSize);
                }
            }

        } lines;

        TextCursor *cursor                     = nullptr;
        std::unique_ptr<TextDocument> document = std::make_unique<TextDocument>(std::list<TextBlock>());
        InputMode *mode                        = nullptr;

        void buildDocument(const UTF8 &text);
        void buildDocument(std::unique_ptr<TextDocument> &&document);
        void buildCursor();
        /// show cursor if cursor should be visible
        void showCursor(bool focus);

        EditMode editMode = EditMode::EDIT;
        [[nodiscard]] bool isMode(EditMode edit) const
        {
            return editMode == edit;
        }
        KeyCode key_signs_remove = KeyCode::KEY_PND;

      public:
        ExpandMode expandMode = ExpandMode::EXPAND_NONE;

      protected:
        TextType textType = TextType::MULTI_LINE;
        /// points to default text font to use
        RawFont *font = nullptr;
        Color textColor;
        bool underline = false;

        bool moveCursor(const NavigationDirection &direction, std::unique_ptr<TextDocument> &document);
        bool handleNavigation(const InputEvent &inputEvent);
        bool handleEnter();

        std::list<DrawCommand *> buildDrawList() override;
        /// redrawing lines
        /// it redraws visible lines on screen and if needed requests resize in parent
        virtual void drawLines();
        /// redrawing cursor
        void drawCursor();

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
        ~Text() override;

        void setEditMode(EditMode mode);
        void setTextType(TextType type);
        void setUnderline(const bool val);
        virtual void setText(const UTF8 &text);
        void setText(std::unique_ptr<TextDocument> &&document);

        void addText(const UTF8 &text);
        void addText(TextBlock text);
        virtual void clear();
        bool isEmpty();
        virtual UTF8 getText();
        /// saves text from widget to file at specified path
        virtual bool saveText(UTF8 path);
        void setFont(const UTF8 &fontName);
        void setFont(RawFont *font);

        // virtual methods from Item
        bool onInput(const InputEvent &inputEvent) override;
        /// move ownership of mode ptr to Text
        void setInputMode(InputMode *&&mode)
        {
            if (this->mode != nullptr) {
                delete this->mode;
            }
            this->mode = mode;
        };
        bool onFocus(bool state) override;
        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
        void setRadius(int value) override;
        void setAlignment(const Alignment &value) override;
        void setPadding(const Padding &value) override;

      private:
        gui::KeyInputMappedTranslation translator;
        bool handleRotateInputMode(const InputEvent &inputEvent);
        bool handleRestoreInputModeUI(const InputEvent &inputEvent);
        bool handleSelectSpecialChar(const InputEvent &inputEvent);
        bool handleActivation(const InputEvent &inputEvent);
        bool handleBackspace(const InputEvent &inputEvent);
        bool handleAddChar(const InputEvent &inputEvent);
        bool handleDigitLongPress(const InputEvent &inputEvent);

        bool addChar(uint32_t utf8);
        bool removeChar();
    };

} /* namespace gui */
