// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"

#include "Text.hpp"
#include "Image.hpp"
#include "ListItem.hpp"
#include <BoxLayout.hpp>
#include "Interface/SMSRecord.hpp"

namespace gui
{

    class SMSInputWidget : public ListItem
    {
        app::ApplicationCommon *application = nullptr;
        HBox *body                          = nullptr;
        gui::Image *replyImage              = nullptr;

      public:
        gui::Text *inputText = nullptr;
        std::optional<SMSRecord> draft; // draft message of the thread we are showing, if exists.
        std::unique_ptr<utils::PhoneNumber::View> number = nullptr;

        SMSInputWidget(app::ApplicationCommon *application);
        ~SMSInputWidget() override = default;

        void handleDraftMessage();
        void clearDraftMessage();
        void updateDraftMessage(const UTF8 &inputText);
        void displayDraftMessage() const;

        auto handleRequestResize(const Item *, Length request_w, Length request_h) -> Size override;
    };

} /* namespace gui */
