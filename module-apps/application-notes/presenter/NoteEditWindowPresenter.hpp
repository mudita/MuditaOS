// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

#include <module-apps/application-notes/model/NotesRepository.hpp>

namespace app::notes
{
    class NoteEditWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;

            virtual bool isNoteEmpty() const noexcept = 0;
        };
        class Presenter : public BasePresenter<NoteEditWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept = default;

            virtual void onNoteChanged()                          = 0;
            virtual bool isAutoSaveApproved() const noexcept      = 0;
            virtual void save(std::shared_ptr<NotesRecord> &note) = 0;
        };
    };

    class NoteEditWindowPresenter : public NoteEditWindowContract::Presenter
    {
      public:
        explicit NoteEditWindowPresenter(std::unique_ptr<AbstractNotesRepository> &&notesRepository);

        void onNoteChanged() override;
        bool isAutoSaveApproved() const noexcept override;
        void save(std::shared_ptr<NotesRecord> &note) override;

      private:
        std::unique_ptr<AbstractNotesRepository> notesRepository;
        bool noteChanged = false;
    };

    class NoteCreateWindowPresenter : public NoteEditWindowPresenter
    {
      public:
        using NoteEditWindowPresenter::NoteEditWindowPresenter;

        bool isAutoSaveApproved() const noexcept override;
    };
} // namespace app::notes
