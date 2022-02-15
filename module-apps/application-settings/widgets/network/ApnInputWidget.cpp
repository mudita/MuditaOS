// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApnInputWidget.hpp"

#include <application-settings/widgets/SettingsStyle.hpp>

#include <Label.hpp>
#include <i18n/i18n.hpp>

namespace gui
{
    ApnInputWidget::ApnInputWidget(settingsInternals::ListItemName listItemName,
                                   std::function<void(const UTF8 &)> navBarTemporaryMode,
                                   std::function<void()> navBarRestoreFromTemporaryMode,
                                   std::function<void()> selectSpecialCharacter,
                                   std::function<void(const std::string &text)> contentChanged,
                                   unsigned int lines)
        : listItemName(listItemName), checkTextContent(std::move(contentChanged))
    {
        setMinimumSize(style::settings::widget::apnInputWidget::w,
                       style::settings::widget::apnInputWidget::title_label_h +
                           style::settings::widget::apnInputWidget::span_size +
                           style::settings::widget::apnInputWidget::input_text_h * lines);

        setMargins(gui::Margins(0, style::margins::huge, 0, 0));

        vBox = new VBox(this, 0, 0, 0, 0);
        vBox->setEdges(RectangleEdge::None);

        titleLabel = new Label(vBox);
        titleLabel->setMinimumSize(style::settings::widget::apnInputWidget::w,
                                   style::settings::widget::apnInputWidget::title_label_h);
        titleLabel->setEdges(RectangleEdge::None);
        titleLabel->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        titleLabel->setFont(style::window::font::verysmall);
        titleLabel->activeItem = false;

        inputText = new TextFixedSize(vBox, 0, 0, 0, 0);
        inputText->setMinimumSize(style::settings::widget::apnInputWidget::w,
                                  style::settings::widget::apnInputWidget::input_text_h * lines);
        inputText->setMargins(Margins(0, style::settings::widget::apnInputWidget::span_size, 0, 0));
        inputText->setUnderlinePadding(style::settings::widget::apnInputWidget::underline_padding);

        inputText->setEdges(RectangleEdge::None);
        inputText->setAlignment(Alignment(gui::Alignment::Horizontal::Left, gui::Alignment::Vertical::Top));
        inputText->setFont(style::window::font::medium);
        inputText->setInputMode(new InputMode(
            {InputMode::Abc, InputMode::ABC, InputMode::abc, InputMode::digit},
            [=](const UTF8 &text) { navBarTemporaryMode(text); },
            [=]() { navBarRestoreFromTemporaryMode(); },
            [=]() { selectSpecialCharacter(); }));
        inputText->setPenFocusWidth(style::window::default_border_focus_w);
        inputText->setPenWidth(style::window::default_border_no_focus_w);
        inputText->setEditMode(EditMode::Edit);

        applyItemNameSpecificSettings();

        focusChangedCallback = [&](Item &item) {
            setFocusItem(focus ? vBox : nullptr);

            if (focus) {
                inputText->setFont(style::window::font::mediumbold);
            }
            else {
                inputText->setFont(style::window::font::medium);
            }
            return true;
        };

        inputCallback = [&](Item &item, const InputEvent &event) {
            auto result = inputText->onInput(event);
            if (checkTextContent != nullptr) {
                checkTextContent(inputText->getText());
            }
            return result;
        };

        dimensionChangedCallback = [&](gui::Item &, const BoundingBox &newDim) -> bool {
            vBox->setArea({0, 0, newDim.w, newDim.h});
            return true;
        };

        setEdges(RectangleEdge::None);
    }

    void ApnInputWidget::applyItemNameSpecificSettings()
    {
        switch (listItemName) {

        case settingsInternals::ListItemName::Name:
            nameHandler();
            break;

        case settingsInternals::ListItemName::APN:
            apnHandler();
            break;

        case settingsInternals::ListItemName::Username:
            usernameHandler();
            break;

        case settingsInternals::ListItemName::Password:
            passwordNumberHandler();
            break;

        case settingsInternals::ListItemName::AuthType:
            authtypeHandler();
            break;

        case settingsInternals::ListItemName::ApnType:
            apntypeHandler();
            break;

        case settingsInternals::ListItemName::ApnProtocol:
            protocolHandler();
            break;

        default:
            LOG_ERROR("Incorrect List Item Name!");
            break;
        }
    }

    void ApnInputWidget::nameHandler()
    {
        titleLabel->setText(utils::translate("app_settings_apn_name"));
        inputText->setTextType(TextType::SingleLine);
        onSaveCallback = [&](std::shared_ptr<packet_data::APN::Config> apnRecord) {
            apnRecord->apn = inputText->getText();
        };
        onLoadCallback = [&](std::shared_ptr<packet_data::APN::Config> apnRecord) {
            inputText->setText(apnRecord->apn);
        };
        onEmptyCallback = [&]() { return inputText->isEmpty(); };
    }

    void ApnInputWidget::apnHandler()
    {
        titleLabel->setText(utils::translate("app_settings_apn_APN"));
        inputText->setTextType(TextType::SingleLine);
        onSaveCallback = [&](std::shared_ptr<packet_data::APN::Config> apnRecord) {
            apnRecord->ip = inputText->getText();
        };
        onLoadCallback = [&](std::shared_ptr<packet_data::APN::Config> apnRecord) {
            inputText->setText(apnRecord->ip);
        };
        onEmptyCallback = [&]() { return inputText->isEmpty(); };
    }

    void ApnInputWidget::usernameHandler()
    {
        titleLabel->setText(utils::translate("app_settings_apn_username"));
        inputText->setTextType(TextType::SingleLine);
        onSaveCallback = [&](std::shared_ptr<packet_data::APN::Config> apnRecord) {
            apnRecord->username = inputText->getText();
        };
        onLoadCallback = [&](std::shared_ptr<packet_data::APN::Config> apnRecord) {
            inputText->setText(apnRecord->username);
        };
        onEmptyCallback = [&]() { return inputText->isEmpty(); };
    }

    void ApnInputWidget::passwordNumberHandler()
    {
        titleLabel->setText(utils::translate("app_settings_apn_password"));
        inputText->setTextType(TextType::SingleLine);
        onSaveCallback = [&](std::shared_ptr<packet_data::APN::Config> apnRecord) {
            apnRecord->password = inputText->getText();
        };
        onLoadCallback = [&](std::shared_ptr<packet_data::APN::Config> apnRecord) {
            inputText->setText(apnRecord->password);
        };
        onEmptyCallback = [&]() { return inputText->isEmpty(); };
    }

    void ApnInputWidget::authtypeHandler()
    {
        titleLabel->setText(utils::translate("app_settings_apn_authtype"));
        inputText->setTextType(TextType::SingleLine);
        onSaveCallback = [&](std::shared_ptr<packet_data::APN::Config> apnRecord) {
            apnRecord->setAuthMethod(inputText->getText());
        };
        onLoadCallback = [&](std::shared_ptr<packet_data::APN::Config> apnRecord) {
            inputText->setText(apnRecord->getAuthMethod());
        };
        onEmptyCallback = [&]() { return inputText->isEmpty(); };
    }

    void ApnInputWidget::apntypeHandler()
    {
        titleLabel->setText(utils::translate("app_settings_apn_apntype"));
        inputText->setTextType(TextType::SingleLine);
        onSaveCallback = [&](std::shared_ptr<packet_data::APN::Config> apnRecord) {
            apnRecord->setApnType(inputText->getText());
        };
        onLoadCallback = [&](std::shared_ptr<packet_data::APN::Config> apnRecord) {
            inputText->setText(apnRecord->getApnType());
        };
        onEmptyCallback = [&]() { return inputText->isEmpty(); };
    }
    void ApnInputWidget::protocolHandler()
    {
        titleLabel->setText(utils::translate("app_settings_apn_apnprotocol"));
        inputText->setTextType(TextType::SingleLine);
        onSaveCallback = [&](std::shared_ptr<packet_data::APN::Config> apnRecord) {
            apnRecord->setApnProtocol(inputText->getText());
        };
        onLoadCallback = [&](std::shared_ptr<packet_data::APN::Config> apnRecord) {
            inputText->setText(apnRecord->getApnProtocol());
        };
        onEmptyCallback = [&]() { return inputText->isEmpty(); };
    }

} /* namespace gui */
