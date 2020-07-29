#pragma once
#include "ImageBox.hpp"
#include "Label.hpp"
#include "Image.hpp"
#include "BoxLayout.hpp"
#include "BottomBar.hpp"

namespace gui
{
    class CheckBox : public ImageBox
    {
        Image *image                                                                                         = nullptr;
        std::function<void(const UTF8 &text, BottomBar::Side side, bool emptyOthers)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode                                              = nullptr;

      public:
        CheckBox(Item *parent,
                 const uint32_t &x,
                 const uint32_t &y,
                 const uint32_t &w,
                 const uint32_t &h,
                 std::function<void(const UTF8 &text, BottomBar::Side side, bool emptyOthers)> bottomBarTemporaryMode =
                     nullptr,
                 std::function<void()> bottomBarRestoreFromTemporaryMode = nullptr);
        virtual ~CheckBox() override = default;
    };

} /* namespace gui */
