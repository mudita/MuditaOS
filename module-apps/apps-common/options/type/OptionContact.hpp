// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "OptionSimple.hpp"
#include "OptionStyle.hpp"
#include "ApplicationCommon.hpp"

#include <module-db/Interface/ContactRecord.hpp>

namespace gui::option
{
    class Contact : public Simple
    {
      private:
        bool onContactOptionClick(app::ApplicationCommon *app,
                                  ContactOperation contactOperation,
                                  const ContactRecord &contactRecord);

      public:
        Contact(app::ApplicationCommon *app,
                ContactOperation contactOperation,
                const ContactRecord &contactRec,
                gui::option::Arrow arrow = option::Arrow::Disabled);
    };
}; // namespace gui::option
