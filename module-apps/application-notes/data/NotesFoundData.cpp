// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesFoundData.hpp"

namespace app::notes
{
    NotesFoundData::NotesFoundData(std::string searchText, std::vector<NotesRecord> notes)
        : gui::SwitchData(std::string{"NotesFoundData"}), searchText{std::move(searchText)}, recordsFound{
                                                                                                 std::move(notes)}
    {}

    const std::string &NotesFoundData::getSearchText() const noexcept
    {
        return searchText;
    }

    const std::vector<NotesRecord> &NotesFoundData::getFoundRecords() const noexcept
    {
        return recordsFound;
    }
} // namespace app::notes
