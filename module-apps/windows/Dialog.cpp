// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "Dialog.hpp"
#include "DialogMetadataMessage.hpp"
#include <service-appmgr/model/ApplicationManager.hpp>
#include <i18n/i18n.hpp>
#include <Image.hpp>

using namespace gui;

namespace style
{
    namespace image
    {
        constexpr uint32_t x = 176;
        constexpr uint32_t y = 132;
    } // namespace image
    namespace text
    {
        constexpr uint32_t x = 40;
        constexpr uint32_t y = 290;
        constexpr uint32_t w = 400;
        constexpr uint32_t h = 132;
    } // namespace text
    namespace no
    {
        constexpr uint32_t x = 75;
        constexpr uint32_t y = 415;
        constexpr uint32_t w = 150;
        constexpr uint32_t h = 75;
    } // namespace no
    namespace yes
    {
        constexpr uint32_t x = 255;
        constexpr uint32_t y = 415;
        constexpr uint32_t w = 150;
        constexpr uint32_t h = 75;
    } // namespace yes
    namespace icontext
    {
        constexpr uint32_t x = image::x;
        constexpr uint32_t y = image::y + 40;
        constexpr uint32_t w = 130;
        constexpr uint32_t h = 100;
    } // namespace icontext

} // namespace style

Dialog::Dialog(app::Application *app, const std::string &name) : gui::AppWindow(app, name)
{
    AppWindow::buildInterface();

    bottomBar->setText(BottomBar::Side::RIGHT, utils::localize.get(style::strings::common::back));

    setTitle("");

    icon = new Image(this, style::image::x, style::image::y, "");

    text = new Text(this, style::text::x, style::text::y, style::text::w, style::text::h);
    text->setRichText("lol");
    text->setTextType(TextType::MultiLine);
    text->setEditMode(EditMode::Browse);
    text->setEdges(RectangleEdge::None);
    text->setFont(style::window::font::medium);
    text->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
}

void Dialog::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (auto metadata = dynamic_cast<DialogMetadataMessage *>(data); metadata != nullptr) {
        setTitle(metadata->get().title);
        text->setRichText(metadata->get().text);
        icon->set(metadata->get().icon);
    }
}

DialogConfirm::DialogConfirm(app::Application *app, const std::string &name) : Dialog(app, name)
{
    bottomBar->setActive(BottomBar::Side::RIGHT, false);
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::ok));
    setFocusItem(icon);
    icon->inputCallback = [=](Item &, const InputEvent &inputEvent) -> bool {
        if (inputEvent.state == InputEvent::State::keyReleasedShort && inputEvent.keyCode == gui::KeyCode::KEY_RF) {
            return true;
        }
        return false;
    };
    setTitle("");
}

void DialogConfirm::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (auto metadata = dynamic_cast<DialogMetadataMessage *>(data); metadata != nullptr) {
        Dialog::onBeforeShow(mode, metadata);
        auto foo                = metadata->get().action;
        icon->activatedCallback = [foo](Item &) -> bool { return foo(); };
    }
}

DialogYesNo::DialogYesNo(app::Application *app, const std::string &name) : Dialog(app, name)
{
    no = new Label(
        this, style::no::x, style::no::y, style::no::w, style::no::h, utils::localize.get(style::strings::common::no));
    no->setPenWidth(0);
    no->setPenFocusWidth(3);
    no->setFilled(false);
    no->setBorderColor(ColorFullBlack);
    no->setEdges(RectangleEdge::Bottom | RectangleEdge::Top);
    no->setFont(style::window::font::big);
    no->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    no->activatedCallback = [=](Item &) -> bool {
        application->returnToPreviousWindow();
        return true;
    };

    yes = new Label(this,
                    style::yes::x,
                    style::yes::y,
                    style::yes::w,
                    style::yes::h,
                    utils::localize.get(style::strings::common::yes));
    yes->setPenWidth(0);
    yes->setPenFocusWidth(3);
    yes->setFilled(false);
    yes->setBorderColor(ColorFullBlack);
    yes->setEdges(RectangleEdge::Bottom | RectangleEdge::Top);
    yes->setFont(style::window::font::big);
    yes->setAlignment(Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));
    yes->activatedCallback = [=](Item &) -> bool { return false; };

    no->setNavigationItem(NavigationDirection::RIGHT, yes);
    yes->setNavigationItem(NavigationDirection::LEFT, no);
    no->setNavigationItem(NavigationDirection::LEFT, yes);
    yes->setNavigationItem(NavigationDirection::RIGHT, no);

    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get("common_confirm"));

    setFocusItem(no);
}

void DialogYesNo::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (auto metadata = dynamic_cast<DialogMetadataMessage *>(data); metadata != nullptr) {
        Dialog::onBeforeShow(mode, metadata);
        auto foo               = metadata->get().action;
        yes->activatedCallback = [=](Item &) -> bool { return foo(); };
        setFocusItem(no);
    }
}

DialogYesNoIconTxt::DialogYesNoIconTxt(app::Application *app, const std::string &name) : DialogYesNo(app, name)
{
    iconText = new Text(this, style::icontext::x, style::icontext::y, style::icontext::w, style::icontext::h);
    iconText->setText("");
    iconText->setTextType(TextType::SingleLine);
    iconText->setEditMode(EditMode::Browse);
    iconText->setEdges(RectangleEdge::None);
    iconText->setFont(style::window::font::largelight);
    iconText->setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::None));
    setFocusItem(no);
}

void DialogYesNoIconTxt::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (auto metadata = dynamic_cast<DialogMetadataMessage *>(data); metadata != nullptr) {
        DialogYesNo::onBeforeShow(mode, metadata);
        iconText->setText(metadata->get().iconText);
        setFocusItem(no);
    }
}

DialogRetry::DialogRetry(app::Application *app, const std::string &name) : Dialog(app, name)
{
    bottomBar->setText(BottomBar::Side::CENTER, utils::localize.get(style::strings::common::retry));
    setFocusItem(bottomBar);
}

void DialogRetry::onBeforeShow(ShowMode mode, SwitchData *data)
{
    if (auto metadata = dynamic_cast<DialogMetadataMessage *>(data); metadata != nullptr) {
        Dialog::onBeforeShow(mode, metadata);
        auto foo                     = metadata->get().action;
        bottomBar->activatedCallback = [foo](Item &) -> bool { return foo(); };
    }
}
