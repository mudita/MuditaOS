#pragma once

#include "Application.hpp"

#include "Text.hpp"
#include "Image.hpp"
#include <BoxLayout.hpp>

namespace gui
{

    class SMSInputWidget : public HBox
    {
        gui::Image *replyImage = nullptr;

      public:
        gui::Text *inputText = nullptr;

        SMSInputWidget(Item *parent, app::Application *application, const utils::PhoneNumber::View &number);
        virtual ~SMSInputWidget() = default;
    };

} /* namespace gui */
