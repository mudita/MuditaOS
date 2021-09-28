// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionCall.hpp"
#include "OptionStyle.hpp"
#include "FontManager.hpp"
#include "Text.hpp"

#include "tools/Common.hpp"
#include <service-appmgr/Controller.hpp>
#include <application-call/data/CallSwitchData.hpp>

namespace gui::option
{
    Call::Call(app::ApplicationCommon *app, const UTF8 &contactName, utils::PhoneNumber::View phoneNumber)
    {
        assert(app != nullptr);

        text              = "<text>" + utils::translate("sms_call_text") + "<b>" + contactName.c_str() + "</b></text>";
        activatedCallback = [app, phoneNumber](gui::Item &item) {
            return app::manager::Controller::sendAction(app,
                                                        app::manager::actions::Call,
                                                        std::make_unique<app::ExecuteCallData>(phoneNumber),
                                                        app::manager::OnSwitchBehaviour::RunInBackground);
        };
    }
} // namespace gui::option
