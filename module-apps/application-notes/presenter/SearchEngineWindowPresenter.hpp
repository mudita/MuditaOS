// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BasePresenter.hpp"

#include <module-apps/application-notes/model/NotesListModel.hpp>

namespace app::notes
{
    class SearchEngineWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() noexcept = default;
            virtual void emptySearch()                                     = 0;
            virtual void processValidSearch(const std::string &searchText) = 0;
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
        SearchEngineWindowPresenter() = default;
        void searchFor(const std::string &searchText) override;
    };
} // namespace app::notes
