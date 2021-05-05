// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-apps/DatabaseModel.hpp>
#include <module-db/Interface/NotesRecord.hpp>
#include <module-apps/Application.hpp>

namespace app::notes
{
    class NotesListItemProvider : public app::DatabaseModel<NotesRecord>, public gui::ListItemProvider
    {
      public:
        explicit NotesListItemProvider(app::Application *app);
    };
} // namespace app::notes
