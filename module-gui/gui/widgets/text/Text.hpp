// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <memory>
#include <vector>

#include "TextConstants.hpp"
#include "utf8/UTF8.hpp"

#include <core/cursors/TextCursor.hpp>
#include <core/cursors/TextLineCursor.hpp>
#include <core/lines/Lines.hpp>
#include <modes/InputMode.hpp>
#include <core/TextDocument.hpp>
#include <parsers/RichTextParser.hpp>

#include <Translator.hpp>
#include <InputEvent.hpp>
#include <Rect.hpp>

namespace gui
{
    struct TextBackup
    {
        TextDocument document;
        unsigned int cursorPos;
    };

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
        friend TextLineCursor;

      protected:
        // holds list of labels for displaying currently visible text lines.

        TextCursor *cursor                      = nullptr;
        CursorStartPosition cursorStartPosition = CursorStartPosition::DocumentEnd;
        std::unique_ptr<TextDocument> document  = std::make_unique<TextDocument>(std::list<TextBlock>());
        InputMode *mode                         = nullptr;
        std::unique_ptr<Lines> lines            = nullptr;

        void buildDocument(const UTF8 &text);
        void buildDocument(std::unique_ptr<TextDocument> &&document);
        void buildCursor();
        TextCursor *createCursor();

        /// show cursor if cursor should be visible
        void showCursor(bool focus);

      public:
        ExpandMode expandMode    = ExpandMode::None;
        EditMode editMode        = EditMode::Edit;
        KeyCode key_signs_remove = KeyCode::KEY_PND;

        [[nodiscard]] bool isMode(EditMode edit) const
        {
            return editMode == edit;
        }

      protected:
        TextType textType = TextType::MultiLine;
        std::list<TextLimit> limitsList;

        /// points to default text font to use
        TextFormat format;

        auto handleRotateInputMode(const InputEvent &inputEvent) -> bool;
        auto handleRestoreInputModeUI(const InputEvent &inputEvent) -> bool;
        auto handleSelectSpecialChar(const InputEvent &inputEvent) -> bool;
        auto handleActivation(const InputEvent &inputEvent) -> bool;
        auto handleNavigation(const InputEvent &inputEvent) -> bool;
        auto handleRemovalChar(const InputEvent &inputEvent) -> bool;
        auto handleWholeTextRemoval(const InputEvent &inputEvent) -> bool;
        auto handleAddChar(const InputEvent &inputEvent) -> bool;
        auto handleLongPressAddChar(const InputEvent &inputEvent) -> bool;

        [[nodiscard]] auto getSizeMinusPadding(Axis axis, Area val) -> Length;
        auto applyParentSizeRestrictions() -> void;
        auto calculateAndRequestSize() -> void;
        auto makePreDrawLines(uint32_t utfVal) -> std::unique_ptr<Lines>;
        auto makePreDrawLines(const TextBlock &textBlock) -> std::unique_ptr<Lines>;

        auto checkMaxSignsLimit(unsigned int limitVal) -> AdditionBound;
        auto checkMaxSignsLimit(const TextBlock &textBlock, unsigned int limitVal)
            -> std::tuple<AdditionBound, TextBlock>;
        auto checkMaxSizeLimit(uint32_t utfVal) -> AdditionBound;
        auto checkMaxSizeLimit(const TextBlock &textBlock) -> std::tuple<AdditionBound, TextBlock>;
        auto checkMaxLinesLimit(uint32_t utfVal, unsigned int limitVal) -> AdditionBound;
        auto checkMaxLinesLimit(const TextBlock &textBlock, unsigned int limitVal)
            -> std::tuple<AdditionBound, TextBlock>;

        void preBuildDrawListHookImplementation(std::list<Command> &commands);
        /// redrawing lines
        /// it redraws visible lines on screen and if needed requests resize in parent
        virtual auto drawLines() -> void;
        /// redrawing cursor
        auto drawCursor() -> void;

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
             ExpandMode expandMode = ExpandMode::None,
             TextType textType     = TextType::MultiLine);
        ~Text() override;

        void setEditMode(EditMode mode);
        void setTextType(TextType type);
        void setTextLimitType(TextLimitType limitType, unsigned int val = 0);
        void clearTextLimits();
        void drawUnderline(bool val);
        virtual void setText(const UTF8 &text);
        void setText(std::unique_ptr<TextDocument> &&document);

        TextBackup backupText() const;
        void restoreFrom(const TextBackup &backup);

        void setTextChangedCallback(TextChangedCallback &&callback);

        void addText(const UTF8 &text, AdditionType additionType = AdditionType::perChar);
        void addText(TextBlock text);
        /// @defgroup richtext can be virtualized by parametrized RichTextParser virtual api ( as second param )
        /// @{
        /// set rich text with default RichTextParser - please see RichTextParser documentation on how to use format
        void setRichText(const UTF8 &text, text::RichTextParser::TokenMap &&tokens = text::RichTextParser::TokenMap{});
        /// add rich text with default RichTextParser - please see RichTextParser documentation on how to use format
        void addRichText(const UTF8 &text, text::RichTextParser::TokenMap &&tokens = text::RichTextParser::TokenMap{});
        /// @}
        virtual void clear();
        bool isEmpty();
        virtual UTF8 getText() const;
        void setFont(const UTF8 &fontName);
        void setFont(RawFont *font);
        void setMinimumWidthToFitText();
        void setMinimumWidthToFitText(const UTF8 &text);
        void setMinimumHeightToFitText(unsigned int linesCount = 1);
        void setColor(Color color);

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

        void accept(GuiVisitor &visitor) override;

      private:
        gui::KeyInputMappedTranslation translator;

      public:
        TextChangedCallback textChangedCallback;

        auto checkAdditionBounds(uint32_t utfVal) -> AdditionBound;
        auto checkAdditionBounds(const TextBlock &textBlock) -> std::tuple<AdditionBound, TextBlock>;

        auto setCursorStartPosition(CursorStartPosition val) -> void;

        bool addChar(uint32_t utf8);
        bool removeChar();
        void onTextChanged();
    };

    char intToAscii(int val);

} /* namespace gui */
