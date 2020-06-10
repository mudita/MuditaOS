#pragma once

#include "BaseThreadItem.hpp"
#include "../models/ThreadModel.hpp"

namespace gui
{

    class ThreadItem : public BaseThreadItem
    {
        ThreadModel *model = nullptr;
        std::shared_ptr<ThreadRecord> thread = nullptr;

        void setPreview();

      public:
        ThreadItem(ThreadModel *model);
        void setThreadItem(std::shared_ptr<ThreadRecord> thread);
        std::shared_ptr<ThreadRecord> getThreadItem()
        {
            return thread;
        }

        static ThreadItem *makeThreadItem(ThreadModel *model, std::shared_ptr<ThreadRecord> thread);
    };

    class ThreadItemWithIndicator : public ThreadItem
    {
        gui::Image *indicator = nullptr;

        void onDimensionChanged_bottom(const BoundingBox &oldDim, const BoundingBox &newDim) override;

      public:
        ThreadItemWithIndicator(ThreadModel *model, const UTF8 &indicatorName);
    };

    class ThreadItemNotRead : public ThreadItemWithIndicator
    {
        static const inline UTF8 indicatorName = "dot_12px_soft";

      public:
        ThreadItemNotRead(ThreadModel *model);
    };

} /*namespace gui*/
