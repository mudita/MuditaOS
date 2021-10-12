// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "popups/Popups.hpp"

#include <service-appmgr/Actions.hpp>
#include <popups/Disposition.hpp>

namespace gui
{
    class PopupRequestParams : public app::manager::actions::ActionParams
    {
      public:
        explicit PopupRequestParams(gui::popup::ID popupId);
        explicit PopupRequestParams(gui::popup::ID popupId, popup::Disposition disposition);
        [[nodiscard]] gui::popup::ID getPopupId() const noexcept;
        [[nodiscard]] const popup::Disposition &getDisposition() const noexcept;
        void setDisposition(const popup::Disposition &d);

      private:
        gui::popup::ID popupId;
        popup::Disposition disposition;
    };
} // namespace gui
