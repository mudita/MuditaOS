#pragma once

#include "ListItem.hpp"
#include "Label.hpp"
#include "Image.hpp"

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
