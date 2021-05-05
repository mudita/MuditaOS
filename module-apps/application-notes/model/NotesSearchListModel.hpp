// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <utility>

#include "NotesListModel.hpp"

namespace app::notes
{

    class NotesSearchListModel : public NotesListModel
    {
      public:
        NotesSearchListModel(app::Application *app, std::shared_ptr<AbstractNotesRepository> notesRepository)
            : NotesListModel(app, std::move(notesRepository))
        {}
        void setSearchText(const std::string &text)
        {
            searchText = text;
        }
        void requestRecords(std::uint32_t offset, std::uint32_t limit) override;
        const std::string &getSearchText() const noexcept
        {
            return searchText;
        }

      private:
        std::string searchText;
    };
} // namespace app::notes
