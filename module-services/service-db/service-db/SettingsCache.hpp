#include <mutex.hpp>
#include "SettingsMessages.hpp"

namespace settings
{
    class SettingsCache
    {
      public:
        const std::string &getValue(const EntryPath &path) const;
        void setValue(const EntryPath &path, const std::string &value);
        static SettingsCache *getInstance();

      private:
        std::map<EntryPath, std::string> settingsMap;
        mutable cpp_freertos::MutexStandard settingsMutex;
    };
} // namespace settings
