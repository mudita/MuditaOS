// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SearchEngineWindowPresenter.hpp"

namespace app::notes
{
    SearchEngineWindowPresenter::SearchEngineWindowPresenter(std::unique_ptr<AbstractNotesRepository> &&notesRepository)
        : notesRepository{std::move(notesRepository)}
    {}

    void SearchEngineWindowPresenter::searchFor(const std::string &searchText)
    {
        if (searchText.empty()) {
            getView()->notesFound({}, searchText);
            return;
        }
        notesRepository->getByText(
            searchText, [searchText, this](const auto &records) { getView()->notesFound(records, searchText); });
    }
} // namespace app::notes
