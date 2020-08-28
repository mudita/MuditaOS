#pragma once

#include "Application.hpp"

#include "Text.hpp"
#include "Image.hpp"
#include <BoxLayout.hpp>
#include <module-db/Interface/ContactRecord.hpp>

namespace gui
{

    class SMSInputWidget : public HBox
    {
        gui::Image *replyImage = nullptr;

      public:
        gui::Text *inputText = nullptr;

        SMSInputWidget(Item *parent, app::Application *application, std::shared_ptr<ContactRecord> contact);
        virtual ~SMSInputWidget() = default;
    };

} /* namespace gui */
