// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <Helpers.hpp>
#include <service-db/EntryPath.hpp>
#include <db/PureFactorySettings.hpp>
#include <service-db/agents/settings/Settings_queries.hpp>

const std::string valid_json   = "{\"serial\" : \"00000000000000\", \"case_colour\" : \"nocase\"}";
const std::string invalid_json = "{\"serial\" : \"00000000000000\", \"case_colour\" : \"nocase\"}}";

constexpr auto schema = "CREATE TABLE IF NOT EXISTS settings_tab ( path TEXT NOT NULL UNIQUE PRIMARY KEY,value TEXT);";

constexpr auto valid_path   = "valid.json";
constexpr auto invalid_path = "invalid.json";

namespace
{
    void spawnAndFillFile(std::filesystem::path path, std::string content)
    {
        if (std::filesystem::exists(path)) {
            std::filesystem::remove(path);
        }

        auto file = std::fopen(valid_path, "w");
        REQUIRE(file != nullptr);
        std::fwrite(content.data(), sizeof(char), content.size(), file);
        std::fclose(file);
    }
} // namespace

TEST_CASE("Factory Settings")
{
    SECTION("valid file")
    {
        spawnAndFillFile(valid_path, valid_json);

        settings::PureFactorySettings factory{valid_path};
        auto entries = factory.getMfgEntries();
        REQUIRE(entries->getRowCount() == 2);
        REQUIRE(entries->getFieldCount() == 2);
        REQUIRE((*entries)[0].getString() == "case_colour");
        REQUIRE((*entries)[1].getString() == "nocase");
    }

    SECTION("invalid file")
    {
        spawnAndFillFile(invalid_path, invalid_json);

        settings::PureFactorySettings factory{invalid_path};
        auto entries = factory.getMfgEntries();
        REQUIRE(entries->getRowCount() == 0);
    }
}

TEST_CASE("Factory Settings Init")
{
    db::tests::DatabaseUnderTest<Database> db{"settings_v2.db"};
    REQUIRE(db.get().query(schema));

    SECTION("Init db with factory entries")
    {
        spawnAndFillFile(valid_path, valid_json);

        settings::PureFactorySettings factory{valid_path};
        factory.initDb(&db.get());

        settings::EntryPath variablePath{
            "", "", "", settings::factory::entry_key + std::string("/serial"), settings::SettingsScope::Global};

        const auto results = db.get().query(settings::Statements::getValue, variablePath.to_string().c_str());
        REQUIRE(results);
        REQUIRE(results->getRowCount() == 1);
    }
}
