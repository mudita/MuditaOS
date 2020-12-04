// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/SwitchData.hpp>

#include <module-db/Interface/NotesRecord.hpp>

namespace app::notes
{
    class NoteSwitchData : public gui::SwitchData
    {
      public:
        explicit NoteSwitchData(NotesRecord record);

        const NotesRecord &getRecord() const noexcept;

      private:
        NotesRecord record;
    };
} // namespace app::notes
