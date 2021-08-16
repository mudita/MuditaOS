// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <apps-common/BasePresenter.hpp>

#include <module-apps/application-notes/model/NotesSearchListModel.hpp>
#include <service-db/DBNotificationMessage.hpp>

namespace app::notes
{
    class NotesSearchWindowContract
    {
      public:
        class View
        {
          public:
            virtual void onNothingFound(const std::string &searchText) = 0;
            virtual void onResultsFilled()                             = 0;
            virtual ~View() noexcept                                   = default;
        };

        class Presenter : public BasePresenter<NotesSearchWindowContract::View>
        {
          public:
            virtual ~Presenter() noexcept                                               = default;
            virtual std::shared_ptr<gui::ListItemProvider> getNotesItemProvider() const = 0;
            virtual void handleSearchResults(db::NotificationMessage *)                 = 0;
            virtual void setSearchText(const std::string &str)                          = 0;
            virtual const std::string &getSearchText() const                            = 0;
        };
    };

    class NotesSearchWindowPresenter : public NotesSearchWindowContract::Presenter
    {
      public:
        explicit NotesSearchWindowPresenter(std::shared_ptr<NotesSearchListModel> notesListItemProvider);

        std::shared_ptr<gui::ListItemProvider> getNotesItemProvider() const override;
        void handleSearchResults(db::NotificationMessage *msg) override;
        virtual void setSearchText(const std::string &text) override;
        virtual const std::string &getSearchText() const override;

      private:
        std::shared_ptr<NotesSearchListModel> notesProvider;
    };
} // namespace app::notes
