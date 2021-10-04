// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Service/Message.hpp>
#include <apps-common/popups/data/PopupRequestParams.hpp>
#include <service-appmgr/Actions.hpp>
#include <service-appmgr/messages/ActionRequest.hpp>

class RebootPopupRequestParams : public gui::PopupRequestParams
{
  public:
    RebootPopupRequestParams() : PopupRequestParams{gui::popup::ID::Reboot}
    {}
};
