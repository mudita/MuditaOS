//
// Created by lucck on 12.10.2020.
//

#pragma once
#ifdef TARGET_Linux
#include <cstdio>
#else
#include "ff_stdio.h"
#endif
#include <unordered_map>
#include <mutex.hpp>
#include <functional>

namespace vfsn::utility
{
    class vfsNotifier
    {
#ifdef TARGET_Linux
        using FILE = std::FILE;
#else
        using FILE = FF_FILE;
#endif
      public:
        //! Event type notified for indexer
        enum class FsEvent
        {
            initialized,
            modified,
            deleted,
            renamed
        };
        // Notification handler type
        using NotifyHandler = std::function<void(std::string_view, FsEvent, std::string_view)>;
        // When file is opened
        auto onFileOpen(const char *filename, const char *mode, const FILE *file) noexcept -> void;
        // When file is closed
        auto onFileClose(const FILE *file) noexcept -> void;
        // When file is removed
        auto onFileRemove(std::string_view filename) noexcept -> void;
        // When file is renamed
        auto onFileRename(std::string_view new_file, std::string_view old_file) -> void;
        // When filesystem is initialized
        auto onFileSystemInitialized() -> void
        {
            notify("/", FsEvent::initialized);
        }
        // Register notification handler
        auto registerNotificationHandler(NotifyHandler hwnd) -> void
        {
            notificationCallback = hwnd;
        }

      private:
        // Notify event
        auto notify(std::string_view file, FsEvent event, std::string_view old_file = "") -> void
        {
            if (notificationCallback)
                notificationCallback(file, event, old_file);
        }

      private:
        // Map for opened handles and paths
        std::unordered_map<const FILE *, std::string> mOpenedMap;
        // Mutex for unordered map
        cpp_freertos::MutexStandard mMutex;
        // Notification handler callback
        NotifyHandler notificationCallback;
    };
} // namespace vfsn::utility
