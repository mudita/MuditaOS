// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "DesktopInputWidget.hpp"
#include <service-cellular/CellularServiceAPI.hpp>


namespace style::desktop
{
    namespace inputWidget
    {
        constexpr gui::Length min_h           = 40;
        constexpr gui::Length max_h           = 50;
        constexpr gui::Length vertical_spacer = 25;
    } // namespace inputWidget

    namespace inputText
    {
        constexpr gui::Length default_input_w = 395;
        constexpr gui::Length max_input_h     = 125;
        constexpr gui::Length default_input_h = 30;
        constexpr gui::Length bottom_padding  = 5;
        constexpr gui::Length left_padding    = 5;
    } // namespace inputText

    namespace body
    {
        constexpr gui::Length body_height = 40;
    } // namespace body

} // namespace style::desktop

namespace gui
{

    DesktopInputWidget::DesktopInputWidget(
        app::ApplicationCommon *application, Item *parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : HBox(parent, x, y, w, h), application(application)
    {
        setMinimumSize(style::window::default_body_width, style::desktop::inputWidget::min_h);
        setMargins(Margins(0, style::desktop::inputWidget::vertical_spacer, 0, 0));
        setEdges(gui::RectangleEdge::None);

        body = new HBox(this, 0, 0, 0, 0);
        body->setEdges(RectangleEdge::Bottom);
        body->setMaximumSize(style::window::default_body_width, style::desktop::body::body_height);
        inputText = new gui::Text(body, 0, 0, 0, 0, ExpandMode::None);
        inputText->setMaximumSize(style::desktop::inputText::default_input_w, style::desktop::inputText::max_input_h);
        inputText->setMinimumSize(style::desktop::inputText::default_input_w,
                                  style::desktop::inputText::default_input_h);
        inputText->setFont(style::window::font::small);
        inputText->setPadding(
            Padding(style::desktop::inputText::left_padding, 0, 0, style::desktop::inputText::bottom_padding));
        inputText->setPenFocusWidth(style::window::default_border_focus_w);
        inputText->setPenWidth(style::window::default_border_focus_w);
        inputText->setEdges(gui::RectangleEdge::None);
        inputText->setEditMode(EditMode::Edit);
        setFocusItem(inputText);

        inputText->activatedCallback = [=](gui::Item &) {
            std::string data = inputText->getText().c_str();
            CellularServiceAPI::USSDRequest(
                this->application, cellular::USSDMessage::RequestType::pullSessionRequest, data);
            inputText->clear();
            application->returnToPreviousWindow();
            return true;
        };

        replyImage = new Image(body, 0, 0, "messages_reply_32px_W_M");
        replyImage->setAlignment(Alignment(gui::Alignment::Vertical::Center));
        replyImage->setMargins(Margins(0, 0, 0, 0));
        replyImage->activeItem = false;

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            body->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };
    }

    auto DesktopInputWidget::handleRequestResize([[maybe_unused]] const Item *child, Length request_w, Length request_h)
        -> Size
    {
        request_h = std::clamp(
            static_cast<Length>(request_h), style::desktop::inputWidget::min_h, style::desktop::inputWidget::max_h);
        setMinimumHeight(request_h);
        if (parent != nullptr) {
            requestSize(request_w, request_h);
        }
        return Size(request_w, request_h);
    }

} /* namespace gui */
