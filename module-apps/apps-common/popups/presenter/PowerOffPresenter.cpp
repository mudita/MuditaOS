// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerOffPresenter.hpp"
#include <service-appmgr/messages/UserPowerDownRequest.hpp>
#include <system/Constants.hpp>

namespace gui
{
    PowerOffPresenter::PowerOffPresenter(app::ApplicationCommon *app) : application(app)
    {}

    void PowerOffPresenter::powerOff()
    {
        auto msg = std::make_shared<app::UserPowerDownRequest>();
        application->bus.sendUnicast(std::move(msg), service::name::system_manager);
    }

} // namespace gui
