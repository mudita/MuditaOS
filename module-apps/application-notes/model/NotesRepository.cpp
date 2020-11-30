// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "NotesRepository.hpp"

#include <module-db/queries/notes/QueryNotesGet.hpp>
#include <module-db/queries/notes/QueryNoteStore.hpp>
#include <module-db/queries/notes/QueryNoteRemove.hpp>

#include <service-db/DBServiceAPI.hpp>

namespace app::notes
{
    NotesDBRepository::NotesDBRepository(Application *application) : application{application}
    {}

    void NotesDBRepository::get(std::uint32_t offset, std::uint32_t limit, const OnGetCallback &callback)
    {
        auto query = std::make_unique<db::query::QueryNotesGet>(offset, limit);
        query->setQueryListener(db::QueryCallback::fromFunction([callback](auto response) {
            auto result = dynamic_cast<db::query::NotesGetResult *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->getRecords(), result->getCount());
            }
            return true;
        }));
        DBServiceAPI::GetQuery(application, db::Interface::Name::Notes, std::move(query));
    }

    void NotesDBRepository::save(const NotesRecord &note, const OnResultCallback &callback)
    {
        auto query = std::make_unique<db::query::QueryNoteStore>(note);
        query->setQueryListener(db::QueryCallback::fromFunction([callback](auto response) {
            auto result = dynamic_cast<db::query::NoteStoreResult *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->succeed());
            }
            return true;
        }));
        DBServiceAPI::GetQuery(application, db::Interface::Name::Notes, std::move(query));
    }

    void NotesDBRepository::remove(const NotesRecord &note, const OnResultCallback &callback)
    {
        auto query = std::make_unique<db::query::QueryNoteRemove>(note.ID);
        query->setQueryListener(db::QueryCallback::fromFunction([callback](auto response) {
            auto result = dynamic_cast<db::query::NoteRemoveResult *>(response);
            if (result == nullptr) {
                return false;
            }
            if (callback) {
                callback(result->succeed());
            }
            return true;
        }));
        DBServiceAPI::GetQuery(application, db::Interface::Name::Notes, std::move(query));
    }
} // namespace app::notes
