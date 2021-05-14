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
        static const std::string empty = "";
        cpp_freertos::LockGuard lock(settingsMutex);
        auto pathIt = settingsMap.find(path);
        if (settingsMap.end() != pathIt) {
            return pathIt->second;
        }
        return empty;
    }

    void SettingsCache::setValue(const EntryPath &path, const std::string &value)
    {
        cpp_freertos::LockGuard lock(settingsMutex);
        settingsMap[path] = value;
    }
} // namespace settings
