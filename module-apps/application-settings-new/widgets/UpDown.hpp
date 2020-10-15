#pragma once

#include <i18/i18.hpp>
#include <BoxLayout.hpp>
#include <InputEvent.hpp>
#include <ProgressBar.hpp>
#include <utf8/UTF8.hpp>

namespace gui
{

    class UpDown : public HBox
    {
      public:
        UpDown(Item *parent,
               int x,
               int y,
               int w,
               int h,
               UTF8 name,
               uint32_t defaultValue,
               uint32_t max = std::numeric_limits<int>::max());

        ~UpDown() override = default;

        void build(UTF8 text);

      private:
        auto setValueOnInput(const InputEvent &event) -> bool;

        Label *label;
        Progress *progress;
        uint32_t value;
        uint32_t maxValue;

        static constexpr uint progress_width = 60;
    };

} // namespace gui
