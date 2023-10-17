// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <service-bluetooth/SettingsHolder.hpp>
#include <BtKeysStorage.hpp>

namespace bluetooth
{
    class TestSettingsHolder : public SettingsHolder
    {
      public:
        std::string entryValue;
        Settings settingType;
        auto getValue(const Settings setting) -> SettingEntry override
        {
            if (setting == settingType) {
                return entryValue;
            }
            else {
                return std::string{};
            }
        }
        void setValue(const Settings &newSetting, const SettingEntry &value) override
        {
            settingType = newSetting;
            entryValue  = std::get<std::string>(value);
        }
    };

} // namespace bluetooth

using namespace bluetooth;

TEST_CASE("BT Keys storage")
{
    KeyStorage storage;
    auto settingHolder = std::make_shared<TestSettingsHolder>();

    storage.settings = settingHolder;

    auto btKeyStorage = storage.getKeyStorage();
    SECTION("put key to the storage")
    {
        bd_addr_t addr;
        sscanf_bd_addr("94:B2:CC:F1:92:B1", addr);
        link_key_t key       = "12345";
        link_key_type_t type = COMBINATION_KEY;
        btKeyStorage->put_link_key(addr, key, type);

        REQUIRE(settingHolder->settingType == Settings::BtKeys);
        REQUIRE(settingHolder->entryValue ==
                "{\"keys\": [{\"bd_addr\": \"94:B2:CC:F1:92:B1\", \"link_key\": \"12345\", \"type\": 0}]}");
    }
    SECTION("get key from the storage")
    {
        bd_addr_t addr;
        sscanf_bd_addr("94:B2:CC:F1:92:B1", addr);
        link_key_t key;
        link_key_type_t type;
        settingHolder->entryValue =
            "{\"keys\": [{\"bd_addr\": \"94:B2:CC:F1:92:B1\", \"link_key\": \"12345\", \"type\": 0}]}";

        btKeyStorage->open();
        auto ret = btKeyStorage->get_link_key(addr, key, &type);

        REQUIRE(std::string(reinterpret_cast<char *>(key)) == "12345");
        REQUIRE(type == COMBINATION_KEY);
        REQUIRE(ret == 1);
    }
    SECTION("get key from the storage when there's no key")
    {
        bd_addr_t addr;
        sscanf_bd_addr("94:B2:CC:F1:92:B1", addr);
        link_key_t key;
        link_key_type_t type;
        settingHolder->entryValue = "";
        btKeyStorage->open();
        btKeyStorage->delete_link_key(addr);
        auto ret = btKeyStorage->get_link_key(addr, key, &type);

        REQUIRE(ret != 1);
    }
    SECTION("put key to the storage few times")
    {
        bd_addr_t addr;
        sscanf_bd_addr("94:B2:CC:F1:92:B1", addr);
        link_key_t key       = "12345";
        link_key_type_t type = COMBINATION_KEY;

        btKeyStorage->open();
        btKeyStorage->put_link_key(addr, key, type);
        btKeyStorage->put_link_key(addr, key, type);
        btKeyStorage->put_link_key(addr, key, type);

        REQUIRE(settingHolder->settingType == Settings::BtKeys);
        REQUIRE(settingHolder->entryValue ==
                "{\"keys\": [{\"bd_addr\": \"94:B2:CC:F1:92:B1\", \"link_key\": \"12345\", \"type\": 0}]}");
    }
    SECTION("delete key from the storage")
    {
        bd_addr_t addr;
        sscanf_bd_addr("94:B2:CC:F1:92:B1", addr);
        settingHolder->entryValue =
            "{\"keys\": [{\"bd_addr\": \"94:B2:CC:F1:92:B1\", \"link_key\": \"12345\", \"type\": 0}]}";

        btKeyStorage->open();
        btKeyStorage->delete_link_key(addr);

        REQUIRE(settingHolder->settingType == Settings::BtKeys);
        REQUIRE(settingHolder->entryValue == "{\"keys\": []}");
    }
    SECTION("double-delete key from the storage")
    {
        bd_addr_t addr;
        sscanf_bd_addr("94:B2:CC:F1:92:B1", addr);
        settingHolder->entryValue =
            "{\"keys\": [{\"bd_addr\": \"94:B2:CC:F1:92:B1\", \"link_key\": \"12345\", \"type\": 0}]}";

        btKeyStorage->open();
        btKeyStorage->delete_link_key(addr);
        btKeyStorage->delete_link_key(addr);

        REQUIRE(settingHolder->settingType == Settings::BtKeys);
        REQUIRE(settingHolder->entryValue == "{\"keys\": []}");
    }
}
