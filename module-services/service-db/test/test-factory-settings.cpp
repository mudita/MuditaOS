// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <service-db/agents/settings/FactorySettings.hpp>
#include <service-db/EntryPath.hpp>

const std::string valid_json   = "{\"serial\" : \"00000000000000\", \"case_colour\" : \"nocase\"}";
const std::string invalid_json = "{\"serial\" : \"00000000000000\", \"case_colour\" : \"nocase\"}}";

const auto valid_path   = "valid.json";
const auto invalid_path = "invalid.json";

TEST_CASE("Factory Settings")
{

    SECTION("valid file")
    {
        auto file = std::fopen(valid_path, "w");
        std::fwrite(valid_json.data(), sizeof(char), valid_json.size(), file);
        std::fclose(file);

        settings::FactorySettings factory{valid_path};
        auto entries = factory.getMfgEntries();
        REQUIRE(entries->getRowCount() == 2);
        REQUIRE(entries->getFieldCount() == 2);
        REQUIRE((*entries)[0].getString() == "case_colour");
        REQUIRE((*entries)[1].getString() == "nocase");
    }

    SECTION("invalid file")
    {
        auto file = std::fopen(invalid_path, "w");
        std::fwrite(invalid_json.data(), sizeof(char), invalid_json.size(), file);
        std::fclose(file);

        settings::FactorySettings factory{invalid_path};
        auto entries = factory.getMfgEntries();
        REQUIRE(entries->getRowCount() == 0);
    }
}

TEST_CASE("Factory Settings Init", "[.]")
{
    Database::initialize();

    SECTION("Init db with factory entries")
    {
        auto file = std::fopen(valid_path, "w");
        std::fwrite(valid_json.data(), sizeof(char), valid_json.size(), file);
        std::fclose(file);

        auto dbPath = purefs::dir::getUserDiskPath() / "settings_v2.db";
        Database db(dbPath.c_str());

        settings::FactorySettings factory{valid_path};
        factory.initDb(&db);

        settings::EntryPath variablePath{
            "", "", "", settings::factory::entry_key + std::string("/serial"), settings::SettingsScope::Global};

        auto results = db.query(settings::Statements::getValue, variablePath.to_string().c_str());
        REQUIRE(results->getRowCount() == 1);
    }
}
