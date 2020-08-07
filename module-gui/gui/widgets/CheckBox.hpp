#pragma once
#include "ImageBox.hpp"
#include "Image.hpp"

namespace gui
{
    class CheckBox : public ImageBox
    {
        // Image *image                                                 = nullptr;
        std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr;
        std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr;

      public:
        CheckBox(Item *parent,
                 const uint32_t &x,
                 const uint32_t &y,
                 const uint32_t &w,
                 const uint32_t &h,
                 Image *image,
                 std::function<void(const UTF8 &text)> bottomBarTemporaryMode = nullptr,
                 std::function<void()> bottomBarRestoreFromTemporaryMode      = nullptr);
        virtual ~CheckBox() override = default;
    };

} /* namespace gui */
