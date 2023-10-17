// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesFoundData.hpp"

namespace app::notes
{
    NotesFoundData::NotesFoundData(std::string searchText)
        : gui::SwitchData(std::string{"NotesFoundData"}), searchText{std::move(searchText)}
    {}

    const std::string &NotesFoundData::getSearchText() const noexcept
    {
        return searchText;
    }
} // namespace app::notes
