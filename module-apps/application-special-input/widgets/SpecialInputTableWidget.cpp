// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <locale>
#include <codecvt>
#include "module-apps/application-special-input/widgets/SpecialInputTableWidget.hpp"
#include "application-special-input/data/SpecialCharactersTableStyle.hpp"
#include <cassert>
#include <module-apps/application-special-input/ApplicationSpecialInput.hpp>
#include <service-appmgr/Controller.hpp>

#include "Style.hpp"
#include "messages/AppMessage.hpp"

namespace gui
{

    using namespace specialInputStyle;

    SpecialInputTableWidget::SpecialInputTableWidget(app::Application *app, std::list<Carrier> &&carrier)
        : application(app)
    {
        setMinimumSize(specialCharacterTableWidget::window_grid_w, specialCharacterTableWidget::window_grid_h);
        setEdges(RectangleEdge::None);

        box = new GridLayout(0,
                             0,
                             specialCharacterTableWidget::window_grid_w,
                             specialCharacterTableWidget::window_grid_h,
                             {specialCharacterTableWidget::char_grid_w, specialCharacterTableWidget::char_grid_h});

        for (auto &carrier : carrier) {
            box->addWidget(carrier.item);
            decorateActionActivated(carrier.item, carrier.val);
        }
        addWidget(box);
        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            return box->onInput(event);
        };

        auto defaultBorderCallback = box->borderCallback;

        box->borderCallback = [defaultBorderCallback](const InputEvent &inputEvent) -> bool {
            if (!inputEvent.isShortPress()) {
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
        auto app = dynamic_cast<app::ApplicationSpecialInput *>(application);
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

    auto generateCharSign(uint32_t val) -> Carrier
    {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
        std::string schar_string = convert.to_bytes(std::u32string(1, val));
        auto el                  = new gui::Label(
            nullptr, 0, 0, specialCharacterTableWidget::char_label_w, specialCharacterTableWidget::char_label_h);
        style::window::decorate(el);
        el->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
        el->setFont(style::window::font::medium);
        el->setText(schar_string);
        return {el, schar_string};
    }
} /* namespace gui */
