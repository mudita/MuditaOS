// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

#include <module-apps/application-notes/model/NotesListModel.hpp>

namespace app::notes
{
    class NotesMainWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
        };
        class Presenter : public BasePresenter<NotesMainWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept = default;

            virtual std::shared_ptr<gui::ListItemProvider> getNotesItemProvider() const = 0;
        };
    };

    class NotesMainWindowPresenter : public NotesMainWindowContract::Presenter
    {
      public:
        explicit NotesMainWindowPresenter(std::shared_ptr<NotesListItemProvider> notesListItemProvider);

        std::shared_ptr<gui::ListItemProvider> getNotesItemProvider() const override;

      private:
        std::shared_ptr<NotesListItemProvider> notesProvider;
    };
} // namespace app::notes
