#pragma once

#include "BaseThreadItem.hpp"
#include "../models/ThreadModel.hpp"

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

        enum class Type
        {
            Regular,
            NotSent,
            NotRead
        };
        static ThreadItem *makeThreadItem(ThreadModel *model, Type type);
    };

    class ThreadItemWithIndicator : public ThreadItem
    {
        gui::Image *indicator = nullptr;

      public:
        ThreadItemWithIndicator(ThreadModel *model, const UTF8 &indicatorName);

        bool onDimensionChanged(const BoundingBox &oldDim, const BoundingBox &newDim) override;
    };

    class ThreadItemNotSent : public ThreadItemWithIndicator
    {
        static const inline UTF8 indicatorName = "messages_error_W_M";

      public:
        ThreadItemNotSent(ThreadModel *model);
    };

    class ThreadItemNotRead : public ThreadItemWithIndicator
    {
        static const inline UTF8 indicatorName = " ";

      public:
        ThreadItemNotRead(ThreadModel *model);
    };

} /*namespace gui*/
