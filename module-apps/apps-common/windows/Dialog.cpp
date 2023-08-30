﻿// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Dialog.hpp"
#include "DialogMetadataMessage.hpp"
#include <i18n/i18n.hpp>
#include <Image.hpp>
#include <apps-common/ApplicationCommon.hpp>

using namespace gui;

Dialog::Dialog(app::ApplicationCommon *app, const std::string &name) : gui::AppWindow(app, name)
{
    AppWindow::buildInterface();
    navBar->setText(nav_bar::Side::Right, utils::translate(style::strings::common::back));
    setTitle("");

    icon = new Icon(this,
                    style::window::default_left_margin,
                    style::window::default_vertical_pos,
                    style::window::default_body_width,
                    style::window::default_body_height,
                    "",
                    "");

    icon->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
    icon->image->setMargins(Margins(0, icon::image_top_margin, 0, icon::image_bottom_margin));
}

void Dialog::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (auto metadata = dynamic_cast<DialogMetadataMessage *>(data); metadata != nullptr) {
        auto title = metadata->get().title;
        if (const auto crLfPos = title.find_first_of("\r\n"); crLfPos != title.npos) {
            const auto lengthToReplace = title.size() - crLfPos;
            title.replace(crLfPos, lengthToReplace, "...");
        }
        setTitle(title);
        icon->text->setRichText(metadata->get().text);
        icon->image->set(metadata->get().icon);
        icon->resizeItems();
    }
}

DialogConfirm::DialogConfirm(app::ApplicationCommon *app, const std::string &name) : Dialog(app, name)
{
    navBar->setActive(nav_bar::Side::Right, false);
    navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::ok));
    setFocusItem(icon);
    icon->inputCallback = [=](Item &, const InputEvent &inputEvent) -> bool {
        return inputEvent.isShortRelease(gui::KeyCode::KEY_RF);
    };
    setTitle("");
}

void DialogConfirm::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (auto metadata = dynamic_cast<DialogMetadataMessage *>(data); metadata != nullptr) {
        Dialog::onBeforeShow(mode, metadata);
        const auto foo          = metadata->get().action;
        icon->activatedCallback = [foo](Item &) -> bool { return foo(); };
    }
}

DialogYesNo::DialogYesNo(app::ApplicationCommon *app, const std::string &name) : Dialog(app, name)
{
    icon->text->setMinimumSize(style::window::default_body_width, dialog::style::option::iconTextH);
    icon->text->setMaximumSize(style::window::default_body_width, dialog::style::option::iconTextH);
    icon->text->setMargins(Margins(0, 0, 0, icon::image_bottom_margin));
    icon->text->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Top));

    hBox = new HBox(icon, 0, 0, 0, 0);
    hBox->setMinimumSize(style::window::default_body_width, dialog::style::option::h);
    hBox->setAlignment(Alignment(gui::Alignment::Horizontal::Center));
    hBox->setEdges(RectangleEdge::None);

    no  = createYesNoOption(hBox, gui::dialog::Option::NO);
    yes = createYesNoOption(hBox, gui::dialog::Option::YES);

    navBar->setText(nav_bar::Side::Center, utils::translate("common_confirm"));
}

Label *DialogYesNo::createYesNoOption(Item *parent, const gui::dialog::Option &optionName)
{
    Label *option = new Label(parent, 0, 0, 0, 0, "");
    if (optionName == gui::dialog::Option::YES) {
        option->setText(utils::translate(style::strings::common::yes));
        option->activatedCallback = [=](Item &) -> bool { return false; };
    }
    else if (optionName == gui::dialog::Option::NO) {
        option->setText(utils::translate(style::strings::common::no));
        option->setMargins(Margins(0, 0, dialog::style::option::margin, 0));
        option->activatedCallback = [=](Item &) -> bool {
            application->returnToPreviousWindow();
            return true;
        };
    }
    option->setMinimumSize(dialog::style::option::w, dialog::style::option::h);
    option->setPenWidth(0);
    option->setPenFocusWidth(3);
    option->setEdges(RectangleEdge::Bottom | RectangleEdge::Top);
    option->setFont(style::window::font::big);
    option->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    return option;
}

void DialogYesNo::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (auto metadata = dynamic_cast<DialogMetadataMessage *>(data); metadata != nullptr) {
        Dialog::onBeforeShow(mode, metadata);
        auto foo               = metadata->get().action;
        yes->activatedCallback = [=](Item &) -> bool { return foo(); };
        hBox->resizeItems();

        no->setNavigationItem(NavigationDirection::RIGHT, yes);
        yes->setNavigationItem(NavigationDirection::LEFT, no);
        no->setNavigationItem(NavigationDirection::LEFT, yes);
        yes->setNavigationItem(NavigationDirection::RIGHT, no);

        setFocusItem(no);
    }
}

DialogYesNoIconTxt::DialogYesNoIconTxt(app::ApplicationCommon *app, const std::string &name) : DialogYesNo(app, name)
{
    iconText = new Label(this,
                         style::window::default_left_margin,
                         style::window::default_vertical_pos,
                         style::window::default_body_width,
                         dialog::style::iconTextLabel::h,
                         "");

    iconText->setText("");
    iconText->setEdges(RectangleEdge::None);
    iconText->setFont(style::window::font::largelight);
    iconText->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
}

void DialogYesNoIconTxt::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (auto metadata = dynamic_cast<DialogMetadataMessage *>(data); metadata != nullptr) {
        DialogYesNo::onBeforeShow(mode, metadata);
        iconText->setText(metadata->get().iconText);
        setFocusItem(no);
    }
}

DialogRetry::DialogRetry(app::ApplicationCommon *app, const std::string &name) : Dialog(app, name)
{
    navBar->setText(nav_bar::Side::Center, utils::translate(style::strings::common::retry));
    setFocusItem(navBar);
}

void DialogRetry::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (auto metadata = dynamic_cast<DialogMetadataMessage *>(data); metadata != nullptr) {
        Dialog::onBeforeShow(mode, metadata);
        auto foo                  = metadata->get().action;
        navBar->activatedCallback = [foo](Item &) -> bool { return foo(); };
    }
}
