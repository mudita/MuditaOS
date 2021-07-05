// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ActiveIconFactory.hpp"

#include <cassert>
#include <i18n/i18n.hpp>
#include <service-appmgr/Controller.hpp>
#include <application-phonebook/data/PhonebookItemData.hpp>
#include <application-call/data/CallSwitchData.hpp>
#include <module-apps/application-messages/data/SMSdata.hpp>

using namespace gui;

ActiveIconFactory::ActiveIconFactory(app::Application *app) : app{app}
{
    assert(app);
}

auto ActiveIconFactory::makeCustomIcon(const UTF8 &image,
                                       std::function<bool(Item &)> onActivated,
                                       std::string bottomBarActivatedName) -> ImageBox *
{
    auto icon = new ImageBox(nullptr, 0, 0, style::widgets::iconsSize, style::widgets::iconsSize, new Image(image));
    icon->activeItem           = onActivated.operator bool();
    icon->activatedCallback    = std::move(onActivated);
    icon->focusChangedCallback = [icon, application = app, name = std::move(bottomBarActivatedName)](Item &item) {
        if (icon->focus) {
            icon->setEdges(RectangleEdge::Bottom | RectangleEdge::Top);
            application->getCurrentWindow()->bottomBarTemporaryMode(
                utils::translate(name), BottomBar::Side::CENTER, false);
        }
        else {
            icon->setEdges(RectangleEdge::None);
        }
        return true;
    };
    return icon;
}

auto ActiveIconFactory::makeSMSIcon(const utils::PhoneNumber::View &number) -> ImageBox *
{
    return makeCustomIcon(
        "messages_notification_icon",
        [application = app, number](gui::Item &item) {
            auto data                        = std::make_unique<SMSSendRequest>(number, std::string{});
            data->ignoreCurrentWindowOnStack = true;
            return app::manager::Controller::sendAction(application,
                                                        app::manager::actions::CreateSms,
                                                        std::move(data),
                                                        app::manager::OnSwitchBehaviour::RunInBackground);
        },
        style::strings::common::send);
}

auto ActiveIconFactory::makeCallIcon(const utils::PhoneNumber::View &number) -> ImageBox *
{
    return makeCustomIcon(
        "phonebook_phone_ringing",
        [application = app, number](gui::Item &item) {
            return app::manager::Controller::sendAction(application,
                                                        app::manager::actions::Call,
                                                        std::make_unique<app::ExecuteCallData>(number),
                                                        app::manager::OnSwitchBehaviour::RunInBackground);
        },
        style::strings::common::call);
}

auto ActiveIconFactory::makeAddContactIcon(const std::shared_ptr<ContactRecord> &contact) -> ImageBox *
{
    return makeCustomIcon(
        "cross",
        [application = app, contact](gui::Item &item) {
            return app::manager::Controller::sendAction(application,
                                                        app::manager::actions::EditContact,
                                                        std::make_unique<PhonebookItemData>(contact),
                                                        app::manager::OnSwitchBehaviour::RunInBackground);
        },
        "common_add");
}
