// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/popups/data/PopupRequestParams.hpp>

class PowerOffPopupRequestParams : public gui::PopupRequestParams
{
  public:
    PowerOffPopupRequestParams() : PopupRequestParams{gui::popup::ID::PowerOff}
    {}
};
