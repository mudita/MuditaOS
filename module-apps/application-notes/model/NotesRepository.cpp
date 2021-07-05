// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesRepository.hpp"

#include <module-db/queries/notes/QueryNotesGet.hpp>
#include <module-db/queries/notes/QueryNotesGetByText.hpp>
#include <module-db/queries/notes/QueryNoteStore.hpp>
#include <module-db/queries/notes/QueryNoteRemove.hpp>

#include <service-db/DBServiceAPI.hpp>

namespace app::notes
{
    NotesDBRepository::NotesDBRepository(Application *application)
        : app::AsyncCallbackReceiver{application}, application{application}
    {}

    void NotesDBRepository::get(std::uint32_t offset, std::uint32_t limit, const OnGetCallback &callback)
    {
        auto query = std::make_unique<db::query::QueryNotesGet>(offset, limit);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Notes);
        task->setCallback([callback](auto response) {
            auto result = dynamic_cast<db::query::NotesGetResult *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->getRecords(), result->getCount());
            }
            return true;
        });
        task->execute(application, this);
    }

    void NotesDBRepository::getByText(const std::string &text,
                                      std::uint32_t offset,
                                      std::uint32_t limit,
                                      const OnGetCallback &callback)
    {
        auto query = std::make_unique<db::query::QueryNotesGetByText>(text, offset, limit);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Notes);
        task->setCallback([callback](auto response) {
            auto result = dynamic_cast<db::query::NotesGetByTextResult *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->getRecords(), result->getCount());
            }
            return true;
        });
        task->execute(application, this);
    }

    void NotesDBRepository::save(const NotesRecord &note, const OnSaveCallback &callback)
    {
        auto query = std::make_unique<db::query::QueryNoteStore>(note);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Notes);
        task->setCallback([callback](auto response) {
            auto result = dynamic_cast<db::query::NoteStoreResult *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->succeed(), result->getNoteId());
            }
            return true;
        });
        task->execute(application, this);
    }

    void NotesDBRepository::remove(const NotesRecord &note, const OnRemoveCallback &callback)
    {
        auto query = std::make_unique<db::query::QueryNoteRemove>(note.ID);
        auto task  = app::AsyncQuery::createFromQuery(std::move(query), db::Interface::Name::Notes);
        task->setCallback([callback](auto response) {
            auto result = dynamic_cast<db::query::NoteRemoveResult *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->succeed());
            }
            return true;
        });
        task->execute(application, this);
    }
} // namespace app::notes
