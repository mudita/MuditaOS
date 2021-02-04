// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionBase.hpp"
#include <module-db/Interface/ContactRecord.hpp>

namespace app
{
    class Application;
}

namespace gui::option
{
    class OptionWithActiveIcons : public Base
    {
      public:
        enum class BasicIcon
        {
            SendSMS,
            Call,
            AddContact
        };
        OptionWithActiveIcons(app::Application *app,
                              std::shared_ptr<ContactRecord> contact,
                              std::vector<BasicIcon> icons);

      private:
        app::Application *app;
        std::shared_ptr<ContactRecord> contact;
        std::vector<BasicIcon> icons;
        [[nodiscard]] auto build() const -> ListItem * override;
    };
} // namespace gui::option
