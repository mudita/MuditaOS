// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <gui/widgets/Image.hpp>
#include <gui/widgets/Label.hpp>
#include <gui/widgets/ListItem.hpp>

#include <chrono>

namespace gui
{
    class MeditationListItem : public ListItem
    {
      protected:
        gui::Label *text = nullptr;

      public:
        MeditationListItem() = delete;
        MeditationListItem(std::string text);
        virtual void select(bool isSelected)
        {}
    };

    class PreparationTimeItem : public MeditationListItem
    {
        gui::Image *imageSelectionTick = nullptr;
        std::chrono::seconds duration;
        [[nodiscard]] static std::string convertToPrintable(std::chrono::seconds _duration);

      public:
        explicit PreparationTimeItem(std::chrono::seconds _duration);
        void select(bool isSelected) final;
        [[nodiscard]] std::chrono::seconds getDuration() const noexcept
        {
            return duration;
        }
    };

    class OptionItemMeditationCounter : public MeditationListItem
    {
        gui::Image *imageOptionOn  = nullptr;
        gui::Image *imageOptionOff = nullptr;

      public:
        explicit OptionItemMeditationCounter(bool isCounterOn);
        void select(bool value) final;
    };

    class OptionItemPreparation : public MeditationListItem
    {
        gui::Image *image = nullptr;

      public:
        OptionItemPreparation();
    };

} // namespace gui
