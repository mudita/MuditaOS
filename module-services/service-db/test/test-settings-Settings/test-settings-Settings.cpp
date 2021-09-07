// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <service-db/Settings.hpp>
#include <service-db/SettingsCache.hpp>
#include <utility>

/// stub
namespace settings
{
    SettingsProxy::~SettingsProxy()
    {}
    SettingsProxy::SettingsProxy(const service::ServiceProxy &interface) : service::ServiceProxy(interface)
    {}
    void SettingsProxy::init(std::function<void(EntryPath, std::string)> onChangeHandler)
    {
        this->onChangeHandler = std::move(onChangeHandler);
    }
    void SettingsProxy::deinit()
    {}

    void SettingsProxy::onChange(EntryPath path, std::string value)
    {
        if (onChangeHandler) {
            onChangeHandler(std::move(path), std::move(value));
        }
    };
    bool SettingsProxy::isValid() const noexcept
    {
        return true;
    }
    void SettingsProxy::registerValueChange(EntryPath){};
    void SettingsProxy::unregisterValueChange(EntryPath){};
    void SettingsProxy::setValue(const EntryPath &path, const std::string &value){};
    std::string SettingsProxy::ownerName()
    {
        return "";
    }

    const std::string &SettingsCache::getValue(const EntryPath &path) const
    {
        static const std::string val;
        return val;
    }
    void SettingsCache::setValue(const EntryPath &path, const std::string &value)
    {}

    SettingsCache *SettingsCache::getInstance()
    {
        static SettingsCache s;
        return &s;
    }

} // namespace settings

/// TODO shall we warn here... on uninitialized settings?
TEST_CASE("Settings - not initialized")
{
    settings::Settings setting;
    SECTION("Not initialized settings")
    {
        auto val = setting.getValue("lol");
        REQUIRE(val.empty());
    }

    SECTION("dead initialized settings")
    {
        auto val = setting.getValue("lol");
        REQUIRE(val.empty());
    }
}

TEST_CASE("Settings - initialized")
{
    /// this will require stubbing
    SECTION("get Value - not exists")
    {}

    SECTION("get Value - exists")
    {}

    SECTION("get Value - different type that expected")
    {}

    SECTION("set value - no value")
    {}

    SECTION("set value - override")
    {}
}
