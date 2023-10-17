// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
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
        explicit NotesFoundData(std::string searchText);

        const std::string &getSearchText() const noexcept;

      private:
        std::string searchText;
    };
} // namespace app::notes
