// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SpecialInputTableWidget.hpp"
#include "ApplicationSpecialInput.hpp"
#include "SpecialCharactersTableStyle.hpp"
#include <service-appmgr/Controller.hpp>
#include <messages/AppMessage.hpp>
#include <Style.hpp>
#include <cassert>
#include <locale>
#include <codecvt>

namespace gui
{

    using namespace specialInputStyle;

    SpecialInputTableWidget::SpecialInputTableWidget(app::ApplicationCommon *app, std::list<Carrier> &&carriers)
        : application(app)
    {
        setMinimumSize(specialCharacterTableWidget::window_grid_w, specialCharacterTableWidget::window_grid_h);
        setEdges(RectangleEdge::None);

        box = new GridLayout(0,
                             0,
                             specialCharacterTableWidget::window_grid_w,
                             specialCharacterTableWidget::window_grid_h,
                             {specialCharacterTableWidget::char_grid_w, specialCharacterTableWidget::char_grid_h});

        for (auto &carrier : carriers) {
            box->addWidget(carrier.item);
            decorateActionActivated(carrier.item, carrier.val);
        }
        addWidget(box);
        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (!event.isShortRelease()) {
                return false;
            }
            return box->onInput(event);
        };

        auto defaultBorderCallback = box->borderCallback;

        box->borderCallback = [defaultBorderCallback](const InputEvent &inputEvent) -> bool {
            if (!inputEvent.isShortRelease()) {
                return false;
            }
            if (inputEvent.is(KeyCode::KEY_UP) || inputEvent.is(KeyCode::KEY_DOWN)) {
                return false;
            }
            else {
                return defaultBorderCallback(inputEvent);
            }
        };

        focusChangedCallback = [&](Item &item) {
            if (getFocusItem() == nullptr) {
                setFocusItem(box);
            }
            return true;
        };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            box->setArea(
                {0, 0, specialCharacterTableWidget::window_grid_w, specialCharacterTableWidget::window_grid_h});
            return true;
        };
    }

    void SpecialInputTableWidget::decorateActionActivated(Item *it, const std::string &str)
    {
        assert(application);
        const auto app = dynamic_cast<app::ApplicationSpecialInput *>(application);
        assert(app);

        it->activatedCallback = [=](Item &it) {
            setFocusItem(nullptr);
            LOG_INFO("handled special char for %s", application->getCurrentWindow()->getName().c_str());

            auto switchData =
                std::make_unique<gui::SwitchSpecialChar>(gui::SwitchSpecialChar::Type::Response, app->requester, str);
            return app::manager::Controller::switchBack(
                application, std::make_unique<app::manager::SwitchBackRequest>(app->requester, std::move(switchData)));
        };
    }

    auto generateNewLineSign() -> Carrier
    {
        auto rect = new Rect(
            nullptr, 0, 0, specialCharacterTableWidget::char_label_w, specialCharacterTableWidget::char_label_h);
        rect->setEdges(gui::RectangleEdge::Top | gui::RectangleEdge::Bottom);
        rect->setPenWidth(0);
        new gui::Image(rect,
                       specialCharacterTableWidget::char_newline_x,
                       specialCharacterTableWidget::char_newline_y,
                       specialCharacterTableWidget::char_label_w,
                       specialCharacterTableWidget::char_label_h,
                       "enter_icon_alpha_W_M");
        return {rect, std::string(&text::newline, 1)};
    }

    auto generateCharSign(std::uint32_t val) -> Carrier
    {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
        const auto scharString = convert.to_bytes(std::u32string(1, val));
        const auto charLabel   = new gui::Label(
            nullptr, 0, 0, specialCharacterTableWidget::char_label_w, specialCharacterTableWidget::char_label_h);
        style::window::decorate(charLabel);
        charLabel->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        charLabel->setFont(style::window::font::medium);
        charLabel->setText(scharString);
        return {charLabel, scharString};
    }
} /* namespace gui */
