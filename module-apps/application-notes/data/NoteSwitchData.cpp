// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#include "NoteSwitchData.hpp"

namespace app::notes
{
    NoteSwitchData::NoteSwitchData(std::shared_ptr<NotesRecord> record)
        : gui::SwitchData(std::string{"NotePreview"}), record{std::move(record)}
    {}

    std::shared_ptr<NotesRecord> NoteSwitchData::getRecord() noexcept
    {
        return record;
    }
} // namespace app::notes
