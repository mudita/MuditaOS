// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NoteEditWindowPresenter.hpp"

namespace app::notes
{
    NoteEditWindowPresenter::NoteEditWindowPresenter(std::unique_ptr<AbstractNotesRepository> &&notesRepository)
        : notesRepository{std::move(notesRepository)}
    {}

    void NoteEditWindowPresenter::save(const NotesRecord &note)
    {
        notesRepository->save(note, nullptr);
    }
} // namespace app::notes
