// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <application-notes/ApplicationNotes.hpp>
#include <module-db/Interface/NotesRecord.hpp>
#include <model/NotesRepository.hpp>

namespace gui
{
    class Option; // Forward declaration.
    class Text;   // Forward declaration.
} // namespace gui

namespace app::notes
{
    std::list<gui::Option> noteListOptions(Application *application,
                                           const NotesRecord &record,
                                           AbstractNotesRepository &notesRepository);
    std::list<gui::Option> notePreviewOptions(Application *application,
                                              const NotesRecord &record,
                                              AbstractNotesRepository &notesRepository,
                                              gui::Text *textWidget);
    std::list<gui::Option> noteEditOptions(Application *application, const NotesRecord &record, gui::Text *textWidget);
} // namespace app::notes
