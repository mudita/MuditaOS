// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
//
#pragma once
#include <unordered_map>
#include <mutex.hpp>
#include <functional>
#include <string>
#include <thread.hpp>

namespace vfsn::utility
{
    class vfsNotifier
    {
        struct FF_FILE;
        using FILE = FF_FILE;

      public:
        //! Default constructor and destructor
        vfsNotifier()                    = default;
        vfsNotifier(const vfsNotifier &) = delete;
        vfsNotifier &operator=(const vfsNotifier &) = delete;
        //! Event type notified for indexer
        enum class FsEvent
        {
            initialized, //! Filesystem is intialized
            modified,    //! File is modified
            deleted,     //! File is deleted
            renamed
        };
        /** Notification handler type
         * Notify function callback defiinition
         */
        using NotifyHandler = std::function<void(std::string_view, FsEvent, std::string_view)>;
        /** This method is called by the vfs layer when vfs open file
         * @param[in] filename Opened file path
         * @param[in] mode Open mode
         * @param[in] Handle to opened file
         * @return None
         */
        auto onFileOpen(std::string_view filename, const char *mode, const FILE *file) noexcept -> void;
        /** This method is called by the vfs layer when vfs close file
         * @param[in] filee Closed file handle
         * @return None
         */
        auto onFileClose(const FILE *file) noexcept -> void;
        /** This method is called by the vfs layer when file is removed
         * @param[in] filename Removed file path
         * @return None
         */
        auto onFileRemove(std::string_view filename) noexcept -> void;
        /** This method is called by the vfs layer when the vfs rename file
         * @param[in] new_file New path name for the file
         * @param[in] old_file Old path for the file
         * @return None
         */
        auto onFileRename(std::string_view new_file, std::string_view old_file) noexcept -> void;
        /** This method is called by the vfs layer when the disk is intiialized
         * @return None
         */
        auto onFileSystemInitialized() noexcept -> void
        {
            notify("/", FsEvent::initialized);
        }
        /** Method for register notification handler
         * @note This function is called from the thread contest which use vfs call
         * @param[in] hwnd Notification handler funtion
         * @return None
         */
        auto registerNotificationHandler(NotifyHandler hwnd) -> void
        {
            notificationCallback = hwnd;
            threadHandle         = hwnd ? cpp_freertos::Thread::GetCurrentThreadHandle() : nullptr;
        }

      private:
        /** Private notification helper internal method
         * @param[in] file Modified file path
         * @param[in] event Notification event type
         * @param[in] old_file Old file path
         * @return None
         */
        auto notify(std::string_view file, FsEvent event, std::string_view old_file = "") -> void
        {
            if (threadHandle != cpp_freertos::Thread::GetCurrentThreadHandle() && notificationCallback)
                notificationCallback(file, event, old_file);
        }

      private:
        //! Map for opened handles and paths
        std::unordered_map<const FILE *, std::string> mOpenedMap;
        //! Mutex for unordered map
        cpp_freertos::MutexStandard mMutex;
        //! Notification handler callback
        NotifyHandler notificationCallback;
        TaskHandle_t threadHandle;
    };
} // namespace vfsn::utility
