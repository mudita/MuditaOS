// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "PopupRequestParamsBase.hpp"

namespace gui
{
    PopupRequestParams::PopupRequestParams(gui::popup::ID popupId) : PopupRequestParams(popupId, popup::Disposition{})
    {}

    PopupRequestParams::PopupRequestParams(gui::popup::ID popupId, popup::Disposition disposition)
        : app::manager::actions::ActionParams{"Popup request parameters"}, popupId{popupId}, disposition(disposition)
    {}

    gui::popup::ID PopupRequestParams::getPopupId() const noexcept
    {
        return popupId;
    }

    const popup::Disposition &PopupRequestParams::getDisposition() const noexcept
    {
        return disposition;
    }

    void PopupRequestParams::setDisposition(const popup::Disposition &d)
    {
        this->disposition = d;
    }
} // namespace gui
