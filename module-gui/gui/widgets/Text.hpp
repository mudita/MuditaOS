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
#include "Lines.hpp"
#include "TextCursor.hpp"
#include "TextDocument.hpp"
#include "TextLine.hpp"
#include "Translator.hpp"
#include "TextLineCursor.hpp"

namespace gui
{
    class Lines;

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
        friend Lines;

      protected:
        // holds list of labels for displaying currently visible text lines.

        TextLineCursor *cursor                 = nullptr;
        std::unique_ptr<TextDocument> document = std::make_unique<TextDocument>(std::list<TextBlock>());
        InputMode *mode                        = nullptr;
        std::unique_ptr<Lines> lines           = nullptr;

        void buildDocument(const UTF8 &text);
        void buildDocument(std::unique_ptr<TextDocument> &&document);
        void buildCursor();
        /// show cursor if cursor should be visible
        void showCursor(bool focus);

      public:
        ExpandMode expandMode    = ExpandMode::EXPAND_NONE;
        EditMode editMode        = EditMode::EDIT;
        KeyCode key_signs_remove = KeyCode::KEY_PND;

        [[nodiscard]] bool isMode(EditMode edit) const
        {
            return editMode == edit;
        }

      protected:
        TextType textType = TextType::MULTI_LINE;
        /// points to default text font to use
        bool underline = false;
        TextFormat format;

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
        /// Callback when text changed
        /// @param `this` item
        /// @param new text
        using TextChangedCallback = std::function<void(Item &, const UTF8 &)>;

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
        void setTextChangedCallback(TextChangedCallback &&callback);

        void addText(const UTF8 &text);
        void addText(TextBlock text);
        /// @defgroup richtext can be virtualized by parametrized RichTextParser virtual api ( as second param )
        /// @{
        /// set rich text with default RichTextParser - please see RichTextParser documentation on how to use format
        void setRichText(const UTF8 &text);
        /// add rich text with default RichTextParser - please see RichTextParser documentation on how to use format
        void addRichText(const UTF8 &text);
        /// @}
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
        [[nodiscard]] auto getTextFormat() const noexcept -> const TextFormat &
        {
            return format;
        }

      private:
        gui::KeyInputMappedTranslation translator;

      public:
        TextChangedCallback textChangedCallback;

        bool handleRotateInputMode(const InputEvent &inputEvent);
        bool handleRestoreInputModeUI(const InputEvent &inputEvent);
        bool handleSelectSpecialChar(const InputEvent &inputEvent);
        bool handleActivation(const InputEvent &inputEvent);
        bool handleBackspace(const InputEvent &inputEvent);
        bool handleDigitLongPress(const InputEvent &inputEvent);
        bool handleAddChar(const InputEvent &inputEvent);

        bool addChar(uint32_t utf8);
        bool removeChar();
        InputBound processBound(InputBound bound, const InputEvent &event);
        void onTextChanged();
    };

    char intToAscii(int val);

} /* namespace gui */
