// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <cstdint>
#include <vector>


#include <module-gui/gui/widgets/ListItemProvider.hpp>

#include "NotesRepository.hpp"
#include "NotesListProvider.hpp"

namespace app::notes
{
    class NotesListModel : public NotesListItemProvider
    {
      public:
        NotesListModel(app::Application *app, std::shared_ptr<AbstractNotesRepository> notesRepository);

        [[nodiscard]] unsigned int requestRecordsCount() override;
        [[nodiscard]] bool updateRecords(std::vector<NotesRecord> records) override;
        void requestRecords(std::uint32_t offset, std::uint32_t limit) override;

        [[nodiscard]] gui::ListItem *getItem(gui::Order order) override;
        [[nodiscard]] unsigned int getMinimalItemSpaceRequired() const override;

      protected:
        bool onNotesRetrieved(const std::vector<NotesRecord> &records, unsigned int notesRepoCount);
        std::shared_ptr<AbstractNotesRepository> notesRepository;
    };
} // namespace app::notes
