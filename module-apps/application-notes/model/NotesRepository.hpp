// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <functional>

#include <apps-common/ApplicationCommon.hpp>

#include <module-db/Interface/NotesRecord.hpp>

namespace app::notes
{
    class AbstractNotesRepository
    {
      public:
        using OnGetCallback    = std::function<bool(const std::vector<NotesRecord> &, unsigned int)>;
        using OnSaveCallback   = std::function<void(bool, std::uint32_t)>;
        using OnRemoveCallback = std::function<void(bool)>;

        virtual ~AbstractNotesRepository() noexcept = default;

        virtual void get(std::uint32_t offset, std::uint32_t limit, const OnGetCallback &callback) = 0;
        virtual void getByText(const std::string &text,
                               std::uint32_t offset,
                               std::uint32_t limit,
                               const OnGetCallback &callback)                                      = 0;
        virtual void save(const NotesRecord &note, const OnSaveCallback &callback)                 = 0;
        virtual void remove(const NotesRecord &note, const OnRemoveCallback &callback)             = 0;
    };

    class NotesDBRepository : public AbstractNotesRepository, public app::AsyncCallbackReceiver
    {
      public:
        explicit NotesDBRepository(ApplicationCommon *application);

        void get(std::uint32_t offset, std::uint32_t limit, const OnGetCallback &callback) override;
        void getByText(const std::string &text,
                       std::uint32_t offset,
                       std::uint32_t limit,
                       const OnGetCallback &callback) override;
        void save(const NotesRecord &note, const OnSaveCallback &callback) override;
        void remove(const NotesRecord &note, const OnRemoveCallback &callback) override;

      private:
        ApplicationCommon *application;
    };
} // namespace app::notes
