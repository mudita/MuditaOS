// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <service-db/SettingsCache.hpp>
#include <mutex.hpp>

namespace settings
{

    SettingsCache *SettingsCache::getInstance()
    {
        static SettingsCache instance;
        return &instance;
    }

    const std::string &SettingsCache::getValue(const EntryPath &path) const
    {
        LOG_ERROR("I gdzie ja niby szukam | %s ?", path.to_string().c_str());

        static const std::string empty = "";
        cpp_freertos::LockGuard lock(settingsMutex);

        for (const auto &elem : settingsMap) {
            LOG_ERROR("Co mam w mapie | %s | %s ?", elem.first.to_string().c_str(), elem.second.c_str());

            if (elem.first == path) {
                LOG_ERROR("Znalazlem KURWA!");
                return elem.second;
            }
        }

        //        auto pathIt = settingsMap.find(path);
        //        if (settingsMap.end() != pathIt) {
        //            LOG_ERROR("Znalazlem kURWA!");
        //            return pathIt->second;
        //        }

        LOG_ERROR("Nie no kurwa nie mam ");
        return empty;
    }

    void SettingsCache::setValue(const EntryPath &path, const std::string &value)
    {
        cpp_freertos::LockGuard lock(settingsMutex);
        settingsMap[path] = value;
    }
} // namespace settings
