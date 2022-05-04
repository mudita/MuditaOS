// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ApplicationSpecialInput.hpp"
#include "SpecialInputMainWindow.hpp"
#include <messages/AppMessage.hpp>

using namespace app;

namespace
{
    constexpr auto SpecialInputAppStackDepth = 512 * 5;
} // namespace

ApplicationSpecialInput::ApplicationSpecialInput(std::string name,
                                                 std::string parent,
                                                 StatusIndicators statusIndicators,
                                                 StartInBackground startInBackground)
    : Application(name, parent, statusIndicators, startInBackground, SpecialInputAppStackDepth)
{
    addActionReceiver(manager::actions::ShowSpecialInput, [this](auto &&data) {
        switchWindow(app::char_select, std::move(data));
        return actionHandled();
    });

    windowsFactory.attach(app::char_select, [](ApplicationCommon *app, const std::string &name) {
        return std::make_unique<gui::SpecialInputMainWindow>(app);
    });
}

sys::MessagePointer ApplicationSpecialInput::DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp)
{
    auto retMsg = Application::DataReceivedHandler(msgl);
    if (dynamic_cast<sys::ResponseMessage *>(retMsg.get())->retCode == sys::ReturnCodes::Success) {
        return retMsg;
    }

    return std::make_shared<sys::ResponseMessage>(sys::ReturnCodes::Unresolved);
}

sys::ReturnCodes ApplicationSpecialInput::InitHandler()
{
    auto ret = Application::InitHandler();
    if (ret != sys::ReturnCodes::Success) {
        LOG_ERROR("Can't initialize ApplicationSpecialInput");
    }
    createUserInterface();

    return ret;
}

void ApplicationSpecialInput::createUserInterface()
{
    windowsFactory.build(this, app::char_select);
}

void ApplicationSpecialInput::destroyUserInterface()
{}
