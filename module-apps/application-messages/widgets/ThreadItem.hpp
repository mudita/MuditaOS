#pragma once

#include "BaseThreadItem.hpp"
#include "ThreadModel.hpp"

namespace gui
{

    class ThreadItem : public BaseThreadItem
    {
        ThreadModel *model = nullptr;
        std::shared_ptr<ThreadRecord> thread = nullptr;

      public:
        ThreadItem(ThreadModel *model);
        void setThreadItem(std::shared_ptr<ThreadRecord> &thread);
        std::shared_ptr<ThreadRecord> getThreadItem()
        {
            return thread;
        }
    };
} /*namespace gui*/
