#pragma once

#include <BoxLayout.hpp>
#include <Image.hpp>
#include <Text.hpp>

namespace gui
{
    class NumberWithIconsWidget
    {
      public:
        NumberWithIconsWidget(gui::Item *parent, const std::string &number);
        ~NumberWithIconsWidget()  = default;

        HBox *hBox        = nullptr;
        Text *numberText  = nullptr;
        Image *phoneImage = nullptr;
        Image *smsImage   = nullptr;
    };

} /* namespace gui */
