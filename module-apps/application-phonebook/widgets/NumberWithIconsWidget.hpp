#pragma once

#include "Application.hpp"

#include <BoxLayout.hpp>
#include <Image.hpp>
#include <ImageBox.hpp>
#include <Text.hpp>

namespace gui
{
    class NumberWithIconsWidget : public HBox
    {
      public:
        NumberWithIconsWidget(app::Application *app,
                              const utils::PhoneNumber::View &number,
                              Item *parent,
                              const uint32_t &x,
                              const uint32_t &y,
                              const uint32_t &w,
                              const uint32_t &h);
        ~NumberWithIconsWidget() override = default;

        Text *numberText  = nullptr;
        ImageBox *phoneImage = nullptr;
        ImageBox *smsImage   = nullptr;
    };

} /* namespace gui */
