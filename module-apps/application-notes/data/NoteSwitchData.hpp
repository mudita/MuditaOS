// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <module-gui/gui/SwitchData.hpp>

#include <module-db/Interface/NotesRecord.hpp>

namespace app::notes
{
    class NoteSwitchData : public gui::SwitchData
    {
      public:
        explicit NoteSwitchData(std::shared_ptr<NotesRecord> record);

        std::shared_ptr<NotesRecord> getRecord() noexcept;

      private:
        std::shared_ptr<NotesRecord> record = nullptr;
    };
} // namespace app::notes
