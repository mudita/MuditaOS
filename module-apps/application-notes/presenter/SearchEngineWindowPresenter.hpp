// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BasePresenter.hpp"

#include <module-apps/application-notes/model/NotesRepository.hpp>

namespace app::notes
{
    class SearchEngineWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;

            virtual void notesFound(const std::vector<NotesRecord> &notes, const std::string &searchText) = 0;
        };
        class Presenter : public BasePresenter<SearchEngineWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept = default;

            virtual void searchFor(const std::string &searchText) = 0;
        };
    };

    class SearchEngineWindowPresenter : public SearchEngineWindowContract::Presenter
    {
      public:
        explicit SearchEngineWindowPresenter(std::unique_ptr<AbstractNotesRepository> &&notesRepository);

        void searchFor(const std::string &searchText) override;

      private:
        std::unique_ptr<AbstractNotesRepository> notesRepository;
    };
} // namespace app::notes
