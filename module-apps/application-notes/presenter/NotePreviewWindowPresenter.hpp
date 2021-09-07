// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

#include <module-apps/application-notes/model/NotesRepository.hpp>

namespace app::notes
{
    class NotePreviewWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };
        class Presenter : public BasePresenter<NotePreviewWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept = default;

            virtual AbstractNotesRepository &getRepository() = 0;
        };
    };

    class NotePreviewWindowPresenter : public NotePreviewWindowContract::Presenter
    {
      public:
        explicit NotePreviewWindowPresenter(std::unique_ptr<AbstractNotesRepository> notesRepository);

        AbstractNotesRepository &getRepository() override;

      private:
        std::unique_ptr<AbstractNotesRepository> notesRepository;
    };
} // namespace app::notes
