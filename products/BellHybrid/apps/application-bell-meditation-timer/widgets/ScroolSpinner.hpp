// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <TextFixedSize.hpp>

namespace gui
{
    class ScroolSpinner : public TextFixedSize
    {
      public:
        using OnUpdateCallback   = std::function<void(int _currentIndex)>;
        using OnGetTitleCallback = std::function<std::string(int _currentIndex)>;

        ScroolSpinner(int size,
                      Boundaries boundaries,
                      OnGetTitleCallback getTitleCB,
                      OnUpdateCallback updateCB = nullptr);

        void setCurrentIndex(int newIndex);
        void setFixedFieldWidth(unsigned char newFixedFieldWidth);
        void setFocusEdges(RectangleEdge edges);

        [[nodiscard]] int getCurrentIndex() const noexcept;
        [[nodiscard]] unsigned char getFixedFieldWidth() const noexcept;

        void stepUp();
        void stepDown();

        // virtual methods from Item
        bool onInput(const InputEvent &inputEvent) override;
        bool onFocus(bool state) override;

      private:
        int size;
        int currentIndex;
        Boundaries boundaries         = Boundaries::Continuous;
        unsigned char fixedFieldWidth = 0; ///< defines number of chars always displayed,
        ///< if current < fixedFieldWidth Label will be filled with 0
        ///< value of 0 means no fixed width.
        RectangleEdge focusEdges = RectangleEdge::Bottom;

        OnGetTitleCallback onGetTitle{nullptr};
        OnUpdateCallback onUpdate{nullptr};

        void updateScroolSpinner();
    };
} // namespace gui
