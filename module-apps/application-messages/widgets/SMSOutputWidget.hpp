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

        auto onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) -> bool override;
        auto handleRequestResize(const Item *, unsigned short request_w, unsigned short request_h) -> Size override;
    };

} /* namespace gui */
