// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "Application.hpp"

#include "Text.hpp"
#include "TextBubble.hpp"
#include "Image.hpp"
#include "ListItem.hpp"
#include <BoxLayout.hpp>

namespace gui
{

    class SMSOutputWidget : public ListItem
    {
        HBox *body            = nullptr;
        TextBubble *smsBubble = nullptr;
        Label *timeLabel      = nullptr;
        Image *errorIcon      = nullptr;

        void timeLabelBuild(time_t timestamp);
        void errorIconBuild();
        void positionTimeLabel() const;

      public:
        gui::Text *inputText = nullptr;

        SMSOutputWidget(app::Application *application, const std::shared_ptr<SMSRecord> &record);
        virtual ~SMSOutputWidget() = default;

        auto handleRequestResize(const Item *, Length request_w, Length request_h) -> Size override;
    };

} /* namespace gui */
