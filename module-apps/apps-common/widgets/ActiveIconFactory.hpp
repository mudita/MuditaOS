// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ImageBox.hpp"
#include <ApplicationCommon.hpp>
#include <module-db/Interface/ContactRecord.hpp>

namespace gui
{
    class ActiveIconFactory
    {
        app::ApplicationCommon *app;

      public:
        explicit ActiveIconFactory(app::ApplicationCommon *app);

        [[nodiscard]] auto makeCustomIcon(const UTF8 &image,
                                          std::function<bool(Item &)> onActivated,
                                          std::string bottomBarActivatedName) -> ImageBox *;
        [[nodiscard]] auto makeSMSIcon(const utils::PhoneNumber::View &number) -> ImageBox *;
        [[nodiscard]] auto makeCallIcon(const utils::PhoneNumber::View &number) -> ImageBox *;
        [[nodiscard]] auto makeAddContactIcon(const std::shared_ptr<ContactRecord> &contact) -> ImageBox *;
    };
} // namespace gui
