#pragma once

#include "ListItem.hpp"
#include "Label.hpp"
#include "Image.hpp"

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

    class PrepTimeItem : public MeditationListItem
    {
        gui::Image *imageSelectionTick = nullptr;

      public:
        explicit PrepTimeItem(std::string text);
        void select(bool isSelected) final;
    };

    class OptionItemMeditationCounter : public MeditationListItem
    {
        gui::Image *imageOptionOn  = nullptr;
        gui::Image *imageOptionOff = nullptr;

      public:
        OptionItemMeditationCounter(std::string text, bool isCounterOn);
        void select(bool value) final;
    };

    class OptionItemPreparation : public MeditationListItem
    {
        gui::Image *image = nullptr;

      public:
        explicit OptionItemPreparation(std::string text);
    };

} // namespace gui
