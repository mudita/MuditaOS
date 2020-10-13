#pragma once

#include "ListItem.hpp"
#include "Label.hpp"
#include "Image.hpp"
#include "Style.hpp"

namespace style::meditation::item
{
    constexpr uint32_t w = style::window::default_body_width;

    namespace img
    {
        constexpr uint32_t x = w - 50;
        constexpr uint32_t y = 22;
    } // namespace img
    namespace text
    {
        constexpr uint32_t x = 37;
        constexpr uint32_t w = 310;
        constexpr uint32_t h = style::window::label::big_h;
    } // namespace text
} // namespace style::meditation::item

namespace gui
{
    class MeditationListItem : public ListItem
    {
      protected:
        gui::Label *text = nullptr;

      public:
        MeditationListItem() = delete;
        MeditationListItem(std::string text);
        virtual void select(bool)
        {}
    };

    class PrepTimeItem : public MeditationListItem
    {
        gui::Image *imageSelectionTick = nullptr;

      public:
        explicit PrepTimeItem(std::string text);
        void select(bool value) final;
    };

    class OptionItem1 : public MeditationListItem
    {
        gui::Image *imageOptionOn  = nullptr;
        gui::Image *imageOptionOff = nullptr;

      public:
        explicit OptionItem1(std::string text);
        void select(bool value) final;
    };

    class OptionItem2 : public MeditationListItem
    {
        gui::Image *image = nullptr;

      public:
        explicit OptionItem2(std::string text);
    };

} // namespace gui
