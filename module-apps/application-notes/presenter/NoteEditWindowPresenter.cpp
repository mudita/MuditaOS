// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NoteEditWindowPresenter.hpp"

namespace app::notes
{
    NoteEditWindowPresenter::NoteEditWindowPresenter(std::unique_ptr<AbstractNotesRepository> &&notesRepository)
        : notesRepository{std::move(notesRepository)}
    {}

    void NoteEditWindowPresenter::save(std::shared_ptr<NotesRecord> &note)
    {
        notesRepository->save(*note, [this, note](bool succeed, std::uint32_t noteId) {
            if (succeed) {
                note->ID = noteId;
            }
        });
    }
} // namespace app::notes
