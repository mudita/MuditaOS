// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <string>

#include <module-db/Interface/NotesRecord.hpp>
#include <module-gui/gui/SwitchData.hpp>

namespace app::notes
{
    class NotesFoundData : public gui::SwitchData
    {
      public:
        NotesFoundData(std::string searchText, std::vector<NotesRecord> notes);

        const std::string &getSearchText() const noexcept;
        const std::vector<NotesRecord> &getFoundRecords() const noexcept;

      private:
        std::string searchText;
        std::vector<NotesRecord> recordsFound;
    };
} // namespace app::notes
