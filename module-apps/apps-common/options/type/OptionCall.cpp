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
    Call::Call(app::Application *app, const ContactRecord &contact)
    {
        assert(app != nullptr);

        text =
            "<text>" + utils::translate("sms_call_text") + "<b>" + contact.getFormattedName().c_str() + "</b></text>";

        activatedCallback = [app, contact](gui::Item &item) {
            if (!contact.numbers.empty()) {
                const auto &phoneNumber = contact.numbers.front().number;
                return app::manager::Controller::sendAction(app,
                                                            app::manager::actions::Call,
                                                            std::make_unique<app::ExecuteCallData>(phoneNumber),
                                                            app::manager::OnSwitchBehaviour::RunInBackground);
            }
            return false;
        };
    }
} // namespace gui::option
