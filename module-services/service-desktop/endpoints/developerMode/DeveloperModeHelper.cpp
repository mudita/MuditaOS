// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <module-sys/Service/Bus.hpp>
#include <module-services/service-evtmgr/Constants.hpp>
#include <module-services/service-desktop/parser/MessageHandler.hpp>
#include "DeveloperModeHelper.hpp"
#include "ParserUtils.hpp" // for parserFSM

namespace parserFSM
{
    class Context;
} // namespace parserFSM

using namespace parserFSM;
auto DeveloperModeHelper::processPutRequest(Context &context) -> sys::ReturnCodes
{
    auto body = context.getBody();
    if (body[json::developerMode::keyPressed].is_number()) {
        auto keyValue = body[json::developerMode::keyPressed].int_value();
        sendKeypress(getKeyCode(keyValue));
        MessageHandler::putToSendQueue(context.createSimpleResponse());
    }
    else {
        context.setResponseStatus(http::Code::BadRequest);
        MessageHandler::putToSendQueue(context.createSimpleResponse());
    }
    return sys::ReturnCodes::Unresolved;
}
auto DeveloperModeHelper::getKeyCode(int val) noexcept -> bsp::KeyCodes
{
    switch (val) {
    case 0:
        return bsp::KeyCodes::NumericKey0;
    case 1:
        return bsp::KeyCodes::NumericKey1;
    case 2:
        return bsp::KeyCodes::NumericKey2;
    case 3:
        return bsp::KeyCodes::NumericKey3;
    case 4:
        return bsp::KeyCodes::NumericKey4;
    case 5:
        return bsp::KeyCodes::NumericKey5;
    case 6:
        return bsp::KeyCodes::NumericKey6;
    case 7:
        return bsp::KeyCodes::NumericKey7;
    case 8:
        return bsp::KeyCodes::NumericKey8;
    case 9:
        return bsp::KeyCodes::NumericKey9;
    case '*':
        return bsp::KeyCodes::NumericKeyAst;
    case '#':
        return bsp::KeyCodes::NumericKeyPnd;
    case 'a':
        return bsp::KeyCodes::JoystickLeft;
    case 'd':
        return bsp::KeyCodes::JoystickRight;
    case 'w':
        return bsp::KeyCodes::JoystickUp;
    case 's':
        return bsp::KeyCodes::JoystickDown;
    case '\n':
        return bsp::KeyCodes::JoystickEnter;
    case 11:
        return bsp::KeyCodes::FnLeft;
    case 12:
        return bsp::KeyCodes::FnRight;
    case 13:
        return bsp::KeyCodes::VolUp;
    case 14:
        return bsp::KeyCodes::VolDown;
    case 15:
        return bsp::KeyCodes::Torch;
    case 16:
        return bsp::KeyCodes::SSwitchUp;
    case 17:
        return bsp::KeyCodes::SSwitchDown;
    case 18:
        return bsp::KeyCodes::SSwitchMid;
    default:
        LOG_ERROR("invalid keycode");
        return bsp::KeyCodes::Undefined;
    };
}
void DeveloperModeHelper::sendKeypress(bsp::KeyCodes keyCode)
{
    auto message          = std::make_shared<sevm::KbdMessage>();
    message->key.key_code = keyCode;
    message->key.state    = RawKey::State::Released;

    sys::Bus::SendUnicast(std::move(message), service::name::evt_manager, ownerServicePtr);
}
