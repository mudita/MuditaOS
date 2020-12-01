// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NoteSwitchData.hpp"

namespace app::notes
{
    NoteSwitchData::NoteSwitchData(NotesRecord record)
        : gui::SwitchData(std::string{"NotePreview"}), record{std::move(record)}
    {}

    const NotesRecord &NoteSwitchData::getRecord() const noexcept
    {
        return record;
    }
} // namespace app::notes
