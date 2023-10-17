// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "NoteEditWindow.hpp"

namespace app::notes
{
    class NoteCreateWindow : public NoteEditWindow
    {
      public:
        NoteCreateWindow(app::ApplicationCommon *app,
                         std::unique_ptr<NoteEditWindowContract::Presenter> &&windowPresenter);

      private:
        bool onInput(const gui::InputEvent &inputEvent) override;
        void onCharactersCountChanged(std::uint32_t count) override;
    };
} // namespace app::notes
