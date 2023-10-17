// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesSearchResultPresenter.hpp"

namespace app::notes
{

    NotesSearchWindowPresenter::NotesSearchWindowPresenter(std::shared_ptr<NotesSearchListModel> notesListItemProvider)
        : notesProvider(std::move(notesListItemProvider))
    {}

    std::shared_ptr<gui::ListItemProvider> NotesSearchWindowPresenter::getNotesItemProvider() const
    {
        return notesProvider;
    }

    void NotesSearchWindowPresenter::handleSearchResults(db::NotificationMessage *msgNotification)
    {
        if (msgNotification->dataModified()) {
            getView()->onResultsFilled();
        }
    }

    void NotesSearchWindowPresenter::setSearchText(const std::string &text)
    {
        notesProvider->setSearchText(text);
    }

    const std::string &NotesSearchWindowPresenter::getSearchText() const
    {
        return notesProvider->getSearchText();
    }
} // namespace app::notes
