// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NoteCreateWindow.hpp"

#include <application-notes/ApplicationNotes.hpp>

namespace app::notes
{
    NoteCreateWindow::NoteCreateWindow(app::ApplicationCommon *app,
                                       std::unique_ptr<NoteEditWindowContract::Presenter> &&windowPresenter)
        : NoteEditWindow(app, std::move(windowPresenter), gui::name::window::note_create)
    {}

    bool NoteCreateWindow::onInput(const gui::InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(gui::KeyCode::KEY_ENTER)) {
            if (!navBar->isActive(gui::nav_bar::Side::Center)) {
                return false;
            }
        }
        return NoteEditWindow::onInput(inputEvent);
    }

    void NoteCreateWindow::onCharactersCountChanged(std::uint32_t count)
    {
        NoteEditWindow::onCharactersCountChanged(count);
        navBar->setActive(gui::nav_bar::Side::Center, count != 0U);
    }
} // namespace app::notes
