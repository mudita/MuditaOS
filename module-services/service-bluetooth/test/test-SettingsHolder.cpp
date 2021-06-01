// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/Settings.hpp>
#include <service-db/SettingsCache.hpp>
#include <catch2/catch.hpp>
#include <SettingsHolder.hpp>
#include <variant>

// this is settings empty stub copied from Settings tets
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

TEST_CASE("Settings Holder")
{
    auto s      = std::make_unique<settings::Settings>();
    auto holder = bluetooth::SettingsHolder(std::move(s));

    SECTION("Single thread use")
    {
        SECTION("initial value")
        {
            auto value = holder.getValue(bluetooth::Settings::State);
            value      = {""};
            SECTION("std::get throws - no value")
            {
                REQUIRE_THROWS(std::get<bool>(value));
            }

            SECTION("std::visit doesn't throw but has something defaut")
            {
                auto visibility = std::visit(bluetooth::BoolVisitor(), value);
                REQUIRE(visibility == false);
            }

            SECTION("std::visit doesn't throw but has something defaut on Visit")
            {
                auto visibility = std::visit(bluetooth::IntVisitor(), value);
                REQUIRE(visibility == 0);
            }
        }

        SECTION("what's set that's set")
        {
            holder.setValue(bluetooth::Settings::Visibility, {"true"});
            auto value = holder.getValue(bluetooth::Settings::Visibility);

            SECTION("with std::visit")
            {
                auto visibility = std::visit(bluetooth::BoolVisitor(), value);
                REQUIRE(visibility == true);
            }

            SECTION("with std::get - success, set string -> variant string")
            {
                REQUIRE(std::get<std::string>(value) == "true");
            }

            SECTION("non matching returns - need to get with visit")
            {
                REQUIRE_THROWS(std::get<bool>(value) != true);
                REQUIRE_THROWS(std::get<int>(value) != 1);
            }
        }
    }
}

// This test is disabled - as it doesn't work as I would except
// instead stateChanged is called when callback to DB is executed
// 1. it doesn't seem intuitive
// 2. it's pass through
// 3. might be problematic
TEST_CASE("Settings Holder - callbacks", "[.]")
{
    auto s      = std::make_unique<settings::Settings>();
    auto holder = bluetooth::SettingsHolder(std::move(s));

    SECTION("state changed cb")
    {
        bool stateChanged    = false;
        holder.onStateChange = [&stateChanged]() { stateChanged = true; };
        auto val             = holder.getValue(bluetooth::Settings::State);

        holder.setValue(bluetooth::Settings::State, {true});

        // !
        REQUIRE(stateChanged == true);
    }
}
