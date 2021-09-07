// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PowerOffPresenter.hpp"
#include <service-appmgr/messages/UserPowerDownRequest.hpp>
#include <module-sys/SystemManager/Constants.hpp>

namespace gui
{
    PowerOffPresenter::PowerOffPresenter(app::Application *app) : application(app)
    {}

    void PowerOffPresenter::powerOff()
    {
        auto msg = std::make_shared<app::UserPowerDownRequest>();
        application->bus.sendUnicast(std::move(msg), service::name::system_manager);
    }

} // namespace gui
