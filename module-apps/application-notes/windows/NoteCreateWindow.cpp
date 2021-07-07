// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NoteCreateWindow.hpp"

namespace app::notes
{
    bool NoteCreateWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            if (!bottomBar->isActive(gui::BottomBar::Side::CENTER)) {
                return false;
            }
        }
        return NoteEditWindow::onInput(inputEvent);
    }

    void NoteCreateWindow::onCharactersCountChanged(std::uint32_t count)
    {
        NoteEditWindow::onCharactersCountChanged(count);
        bottomBar->setActive(gui::BottomBar::Side::CENTER, count != 0U);
    }
} // namespace app::notes
