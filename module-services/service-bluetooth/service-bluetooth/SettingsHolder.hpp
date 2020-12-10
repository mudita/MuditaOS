// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <json/json11.hpp>
#include <string>
#include <variant>
#include <service-db/Settings.hpp>
#include <agents/settings/SystemSettings.hpp>
#include <module-utils/Utils.hpp>

namespace Bluetooth
{

    enum class Settings
    {
        DeviceName,
        Visibility,
        Power,
        BondedDevices
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

    class SettingsHolder
    {
      public:
        explicit SettingsHolder(std::unique_ptr<settings::Settings> settingsPtr);
        auto getValue(const Settings setting) -> SettingEntry;
        void setValue(const Settings &newSetting, const SettingEntry &value);

      private:
        static std::map<Settings, std::string> settingString;
        std::unique_ptr<settings::Settings> settingsProvider;
        std::map<Settings, SettingEntry> settingsMap;
    };
} // namespace Bluetooth
