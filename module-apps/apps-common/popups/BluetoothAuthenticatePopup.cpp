// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BluetoothAuthenticatePopup.hpp"

#include <ApplicationCommon.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    namespace bluetooth::authenticate
    {
        inline constexpr auto option_box_top_margin = 40U;

        inline constexpr auto max_passkey_signs_count = 16U;
        inline constexpr auto min_passkey_signs_count = 4U;
        inline constexpr auto passkey_input_w         = 300U;
        inline constexpr auto passkey_input_h         = 60U;

        inline constexpr auto buttons_input_w = 150U;
        inline constexpr auto buttons_input_h = 75U;
        inline constexpr auto buttons_margin  = 30U;

        enum ButtonsOptions
        {
            Pair,
            Cancel,
        };

    } // namespace bluetooth::authenticate

    BluetoothAuthenticatePopup::BluetoothAuthenticatePopup(app::ApplicationCommon *app, const std::string &name)
        : AppWindow(app, name)
    {
        buildInterface();
    }

    void BluetoothAuthenticatePopup::buildInterface()
    {
        AppWindow::buildInterface();

        setTitle(utils::translate("bluetooth_popup"));

        navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));

        infoIcon = new gui::Icon(this,
                                 style::window::default_left_margin,
                                 style::window::default_vertical_pos,
                                 style::window::default_body_width,
                                 style::window::default_body_height,
                                 "bluetooth_128px_W_G",
                                 utils::translate(""));
        infoIcon->setAlignment(Alignment::Horizontal::Center);
    }

    void BluetoothAuthenticatePopup::onBeforeShow(ShowMode mode, SwitchData *data)
    {
        auto temp          = dynamic_cast<BluetoothAuthenticateRequestParams *>(data);
        authenticateParams = new BluetoothAuthenticateRequestParams(
            temp->getDevice(), temp->getAuthenticateType(), temp->getPairingCode());

        if (authenticateParams == nullptr) {
            LOG_ERROR("No authenticate data received");
            return;
        }

        switch (authenticateParams->getAuthenticateType()) {
        case ::bluetooth::AuthenticateType::Pin:
        case ::bluetooth::AuthenticateType::Passkey:
            createTextAuthenticateInput();
            break;
        case ::bluetooth::AuthenticateType::PairCancel:
            createButtonsAuthenticateInput();
            break;
        }
    }

    void BluetoothAuthenticatePopup::createTextAuthenticateInput()
    {
        inputText = new TextFixedSize(infoIcon, 0, 0, 0, 0);
        inputText->drawUnderline(false);
        inputText->setTextType(TextType::SingleLine);
        inputText->setTextEllipsisType(TextEllipsis::Both);
        inputText->setFont(style::window::font::largelight);
        inputText->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        inputText->setEdges(RectangleEdge::Bottom);
        inputText->setInputMode(new InputMode({InputMode::digit}));
        inputText->setPenWidth(style::window::default_border_rect_no_focus);
        inputText->setMinimumSize(bluetooth::authenticate::passkey_input_w, bluetooth::authenticate::passkey_input_h);
        inputText->setMargins({0, bluetooth::authenticate::option_box_top_margin, 0, 0});

        if (authenticateParams->getAuthenticateType() == ::bluetooth::AuthenticateType::Passkey) {
            infoIcon->text->setText(utils::translate("bluetooth_popup_passkey"));
            inputText->setTextLimitType(TextLimitType::MaxSignsCount, bluetooth::authenticate::max_passkey_signs_count);

            inputText->activatedCallback = [&](Item &) -> bool {
                auto passkey = inputText->getText();
                if (passkey.length() >= bluetooth::authenticate::min_passkey_signs_count) {
                    application->bus.sendUnicast(std::make_shared<message::bluetooth::ResponseAuthenticatePasskey>(
                                                     passkey, authenticateParams->getDevice()),
                                                 service::name::bluetooth);
                    return true;
                }
                return false;
            };
        }
        else if (authenticateParams->getAuthenticateType() == ::bluetooth::AuthenticateType::Pin) {
            infoIcon->text->setText(utils::translate("bluetooth_popup_pin"));
            inputText->setTextLimitType(TextLimitType::MaxSignsCount, bluetooth::authenticate::min_passkey_signs_count);

            inputText->activatedCallback = [&](Item &) -> bool {
                auto pin = inputText->getText();
                if (pin.length() == bluetooth::authenticate::min_passkey_signs_count) {
                    application->bus.sendUnicast(std::make_shared<message::bluetooth::ResponseAuthenticatePin>(
                                                     pin, authenticateParams->getDevice()),
                                                 service::name::bluetooth);
                    return true;
                }
                return false;
            };
        }

        infoIcon->resizeItems();

        setFocusItem(inputText);
    }

    auto BluetoothAuthenticatePopup::onInput(const InputEvent &inputEvent) -> bool
    {
        auto ret = AppWindow::onInput(inputEvent);

        if (authenticateParams->getAuthenticateType() == ::bluetooth::AuthenticateType::Passkey ||
            authenticateParams->getAuthenticateType() == ::bluetooth::AuthenticateType::Pin) {

            if (inputText->getText().length() >= bluetooth::authenticate::min_passkey_signs_count) {
                navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::confirm));
            }
            else {
                navBar->setActive(nav_bar::Side::Center, false);
            }
        }
        return ret;
    }

    void BluetoothAuthenticatePopup::createButtonsAuthenticateInput()
    {
        auto pairingCode = authenticateParams->getPairingCode();

        if (pairingCode.has_value()) {
            infoIcon->text->setRichText(utils::translate("bluetooth_popup_pair_cancel_code"),
                                        {{"$DEVICE", std::string(authenticateParams->getDevice().name.data())},
                                         {"$CODE", utils::to_string(pairingCode.value())}});
        }
        else {
            infoIcon->text->setRichText(utils::translate("bluetooth_popup_pair_cancel_no_code"),
                                        {{"$DEVICE", std::string(authenticateParams->getDevice().name.data())}});
        }

        navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::confirm));

        auto hBox = new HBox(infoIcon, 0, 0, 0, 0);
        hBox->setMinimumSize(style::window::default_body_width, bluetooth::authenticate::buttons_input_h);
        hBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
        hBox->setEdges(RectangleEdge::None);
        hBox->setMargins({0, bluetooth::authenticate::option_box_top_margin, 0, 0});

        auto createButton = [&](gui::bluetooth::authenticate::ButtonsOptions optionName) {
            Label *option = new Label(hBox, 0, 0, 0, 0, "");

            if (optionName == gui::bluetooth::authenticate::ButtonsOptions::Pair) {
                option->setText(utils::translate("bluetooth_popup_pair"));
                option->activatedCallback = [&](Item &) -> bool {
                    application->bus.sendUnicast(std::make_shared<message::bluetooth::ResponseAuthenticatePairCancel>(
                                                     true, authenticateParams->getDevice()),
                                                 service::name::bluetooth);
                    return true;
                };
            }
            else if (optionName == gui::bluetooth::authenticate::ButtonsOptions::Cancel) {
                option->setText(utils::translate("bluetooth_popup_cancel"));
                option->setMargins({0, 0, gui::bluetooth::authenticate::buttons_margin, 0});
                option->activatedCallback = [=](Item &) -> bool {
                    application->bus.sendUnicast(std::make_shared<message::bluetooth::ResponseAuthenticatePairCancel>(
                                                     false, authenticateParams->getDevice()),
                                                 service::name::bluetooth);
                    application->returnToPreviousWindow();
                    return true;
                };
            }

            option->setMinimumSize(bluetooth::authenticate::buttons_input_w, bluetooth::authenticate::buttons_input_h);
            option->setPenWidth(0);
            option->setPenFocusWidth(3);
            option->setEdges(RectangleEdge::Bottom | RectangleEdge::Top);
            option->setFont(style::window::font::big);
            option->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

            return option;
        };

        createButton(bluetooth::authenticate::Cancel);
        createButton(bluetooth::authenticate::Pair);

        infoIcon->resizeItems();

        setFocusItem(hBox);
    }

} // namespace gui
