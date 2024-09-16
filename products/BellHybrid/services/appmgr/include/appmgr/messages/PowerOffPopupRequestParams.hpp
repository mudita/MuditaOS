// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

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
