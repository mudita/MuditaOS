// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <apps-common/DatabaseModel.hpp>
#include <module-db/Interface/NotesRecord.hpp>
#include <apps-common/ApplicationCommon.hpp>

namespace app::notes
{
    class NotesListItemProvider : public app::DatabaseModel<NotesRecord>, public gui::ListItemProvider
    {
      public:
        explicit NotesListItemProvider(app::ApplicationCommon *app);
    };
} // namespace app::notes
