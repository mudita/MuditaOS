#pragma once

#include "Application.hpp"

#include <BoxLayout.hpp>
#include <Image.hpp>
#include <Text.hpp>

namespace gui
{
    class NumberWithIconsWidget : public HBox
    {
      public:
        NumberWithIconsWidget(app::Application *app, gui::Item *parent, const utils::PhoneNumber::View &number);
        ~NumberWithIconsWidget() override = default;

        Text *numberText  = nullptr;
        Image *phoneImage = nullptr;
        Image *smsImage   = nullptr;
    };

} /* namespace gui */
