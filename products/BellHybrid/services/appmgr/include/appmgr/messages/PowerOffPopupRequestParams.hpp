// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <apps-common/popups/data/PopupRequestParams.hpp>
#include <service-appmgr/Actions.hpp>
#include <service-appmgr/messages/ActionRequest.hpp>

class PowerOffPopupRequestParams : public gui::PopupRequestParams
{
  public:
    PowerOffPopupRequestParams() : PopupRequestParams{gui::popup::ID::PowerOff}
    {}
};
