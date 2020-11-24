// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesMainWindowPresenter.hpp"

namespace app::notes
{
    NotesMainWindowPresenter::NotesMainWindowPresenter(std::shared_ptr<NotesProvider> notesProvider)
        : notesProvider{std::move(notesProvider)}
    {}

    std::shared_ptr<gui::ListItemProvider> NotesMainWindowPresenter::getNotesItemProvider() const
    {
        return notesProvider;
    }

    std::shared_ptr<DatabaseModel<NotesRecord>> NotesMainWindowPresenter::getNotesDAO() const
    {
        return notesProvider;
    }
} // namespace app::notes
