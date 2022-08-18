// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>

#include <filesystem>
#include <Tables/NotesTable.hpp>
#include "Database/Database.hpp"
#include "Databases/NotesDB.hpp"

TEST_CASE("Notes Table tests")
{
    Database::initialize();

    const auto notesDbPath = std::filesystem::path{"sys/user"} / "notes.db";
    NotesDB notesDb{notesDbPath.c_str()};
    REQUIRE(notesDb.isInitialized());

    NotesTable table{&notesDb};
    table.removeAll();
    REQUIRE(table.count() == 0);

    constexpr auto testSnippet = "TEST SNIPPET";

    NotesTableRow rowIn;
    rowIn.snippet = testSnippet;

    table.add(rowIn);
    REQUIRE(table.count() == 1);

    SECTION("Get notes")
    {
        const auto &records = table.getLimitOffset(0, table.count());
        REQUIRE(records.size() == table.count());
        REQUIRE(records[0].snippet == testSnippet);
    }

    SECTION("Get notes by text query")
    {
        constexpr auto testSearch   = "TEST";
        const auto [records, count] = table.getByText(testSearch, 0, 1);
        REQUIRE(records.size() == 1);
        REQUIRE(records[0].snippet == testSnippet);
    }

    SECTION("Add a note")
    {
        NotesTableRow row;
        row.snippet = testSnippet;
        table.add(row);
        REQUIRE(table.count() == 2);
    }

    SECTION("Update a note")
    {
        constexpr auto testId             = 1;
        constexpr auto testSnippetUpdated = "UPDATED TEST SNIPPET";
        NotesTableRow row;
        row.ID      = testId;
        row.snippet = testSnippetUpdated;
        table.update(row);
        REQUIRE(table.count() == 1);

        const auto &record = table.getById(testId);
        REQUIRE(record.snippet == testSnippetUpdated);
    }

    SECTION("Remove a note")
    {
        table.removeById(1);
        REQUIRE(table.count() == 0);
    }

    Database::deinitialize();
}
