// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>
#include <notifications/NotificationsPresenter.hpp>

namespace gui
{
    class WallpaperBase
    {
      protected:
        Item *parent = nullptr;

      public:
        explicit WallpaperBase(Item *parent);
        virtual void show()                                                         = 0;
        virtual void hide()                                                         = 0;
        virtual std::shared_ptr<NotificationsPresenter> getNotificationsPresenter() = 0;
    };

} /* namespace gui */
