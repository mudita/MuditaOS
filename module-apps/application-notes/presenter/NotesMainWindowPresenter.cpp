// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesMainWindowPresenter.hpp"

namespace app::notes
{
    NotesMainWindowPresenter::NotesMainWindowPresenter(std::shared_ptr<NotesListItemProvider> notesProvider)
        : notesProvider{std::move(notesProvider)}
    {}

    std::shared_ptr<gui::ListItemProvider> NotesMainWindowPresenter::getNotesItemProvider() const
    {
        return notesProvider;
    }

    bool NotesMainWindowPresenter::isNoteListEmpty()
    {
        return notesProvider->requestRecordsCount() == 0U;
    }

    bool NotesMainWindowPresenter::updateNotes(std::vector<NotesRecord> &&records)
    {
        return notesProvider->updateRecords(std::move(records));
    }
} // namespace app::notes
