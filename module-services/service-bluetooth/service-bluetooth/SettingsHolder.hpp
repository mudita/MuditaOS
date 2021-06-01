// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#pragma once
#include <json11.hpp>
#include <string>
#include <variant>
#include <service-db/Settings.hpp>
#include <agents/settings/SystemSettings.hpp>
#include <module-utils/Utils.hpp>
#include "SettingsSerializer.hpp"

namespace bluetooth
{

    enum class Settings
    {
        DeviceName,
        Visibility,
        State,
        BondedDevices,
        BtKeys,
        ConnectedDevice
    };

    constexpr inline auto trueStr  = "true";
    constexpr inline auto falseStr = "false";

    using SettingEntry = std::variant<int, bool, std::string>;
    struct StringVisitor
    {
        auto operator()(int input) const -> std::string
        {
            return utils::to_string(input);
        }
        auto operator()(bool input) const -> std::string
        {
            return input ? trueStr : falseStr;
        }
        auto operator()(const std::string &input) const -> std::string
        {
            return input;
        }
    };

    struct IntVisitor
    {
        auto operator()(const std::string &input) const -> int
        {
            int value = 0;
            if (not utils::toNumeric(input, value)) {
                LOG_FATAL("Fail on: %s", input.c_str());
                throw std::runtime_error("utils::toNumeric failed");
            }
            return value;
        }

        /// please do not remove to avoid implicit char* to bool conversion
        /// which with char* would check bool operator and return on non null ptr
        auto operator()(const char *input) const -> int
        {
            if (input == nullptr) {
                throw std::runtime_error("utils::toNumeric failed - nullptr");
            }
            return operator()(std::string(input));
        }

        auto operator()(bool input) const -> int
        {
            return static_cast<int>(input);
        }

        auto operator()(int input) const -> int
        {
            return input;
        }
    };

    /// Please mind that :
    /// bool visitor returns true when:
    /// - there is trueStr
    /// - there is non zero integral value in ( default bool behaviour, the same as with int visitation in
    /// operator(int))
    struct BoolVisitor
    {

        // - should we check: atoi != 0 ?
        // - should we check to_lower TRUE / True?
        auto operator()(const std::string &input) const -> bool
        {
            auto data = input;
            std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) { return std::tolower(c); });
            if (data == trueStr) {
                return true;
            }
            if (data == falseStr) {
                return false;
            }
            int value    = 0;
            auto success = utils::toNumeric(data, value);
            if (not success) {
                return false;
            }
            return value != 0;
        }

        /// please do not remove to avoid implicit char* to bool conversion
        /// which with char* would check bool operator and return on non null ptr
        auto operator()(const char *input) const -> bool
        {
            if (input == nullptr) {
                throw std::runtime_error("bool visitor nullptr c_str");
            }
            return operator()(std::string{input});
        }

        auto operator()(bool input) const -> bool
        {
            return input;
        }
        auto operator()(int input) const -> bool
        {
            return input != 0;
        }
    };

    class SettingsHolder
    {
      public:
        explicit SettingsHolder(std::unique_ptr<settings::Settings> settingsPtr);
        auto getValue(const Settings setting) -> SettingEntry;
        void setValue(const Settings &newSetting, const SettingEntry &value);
        std::function<void()> onStateChange;
        std::function<void(const std::string &)> onLinkKeyAdded;
        void deinit();

      private:
        static std::map<Settings, std::string> settingString;
        std::unique_ptr<settings::Settings> settingsProvider;
        std::map<Settings, SettingEntry> settingsMap;
    };
} // namespace Bluetooth
