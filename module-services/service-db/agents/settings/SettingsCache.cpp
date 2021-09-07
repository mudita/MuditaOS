// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/SettingsCache.hpp>
#include <mutex.hpp>

namespace settings
{

    namespace
    {
        class SettingsCacheImpl : public SettingsCache
        {
          public:
            static SettingsCacheImpl &get()
            {
                static SettingsCacheImpl instance;
                return instance;
            }

            const std::string &getValue(const EntryPath &path) const;
            void setValue(const EntryPath &path, const std::string &value);

          private:
            std::map<EntryPath, std::string> settingsMap;
            mutable cpp_freertos::MutexStandard settingsMutex;
        };

        const std::string &SettingsCacheImpl::getValue(const EntryPath &path) const
        {
            static const std::string empty;
            cpp_freertos::LockGuard lock(settingsMutex);
            auto pathIt = settingsMap.find(path);
            if (settingsMap.end() != pathIt) {
                return pathIt->second;
            }
            return empty;
        }

        void SettingsCacheImpl::setValue(const EntryPath &path, const std::string &value)
        {
            cpp_freertos::LockGuard lock(settingsMutex);
            settingsMap[path] = value;
        }
    } // namespace

    SettingsCache *SettingsCache::getInstance()
    {
        return &SettingsCacheImpl::get();
    }

    const std::string &SettingsCache::getValue(const EntryPath &path) const
    {
        return SettingsCacheImpl::get().getValue(path);
    }

    void SettingsCache::setValue(const EntryPath &path, const std::string &value)
    {
        return SettingsCacheImpl::get().setValue(path, value);
    }
} // namespace settings
