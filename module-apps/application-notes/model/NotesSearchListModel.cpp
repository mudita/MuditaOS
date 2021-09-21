// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesSearchListModel.hpp"
#include <log/log.hpp>

namespace app::notes
{
    void NotesSearchListModel::requestRecords(std::uint32_t offset, std::uint32_t limit)
    {
        notesRepository->getByText(
            searchText, offset, limit, [this](const std::vector<NotesRecord> &records, unsigned int notesRepoCount) {
                return onNotesRetrieved(records, notesRepoCount);
            });
    }
} // namespace app::notes
