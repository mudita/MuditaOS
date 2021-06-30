// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "ThreadsModel.hpp"

#include <BaseThreadItem.hpp>
#include <ContactRecord.hpp>

namespace gui
{

    class ThreadItem : public BaseThreadItem
    {
        std::shared_ptr<ThreadListStruct> threadStruct;

        auto getNumberImportance() -> std::optional<long int>;

        void setPreview();
        void setContactName(std::optional<long int> numberImportance = std::nullopt);

      public:
        ThreadItem() = default;

        void setThreadItem(std::shared_ptr<ThreadListStruct> _threadStruct);

        [[nodiscard]] auto getThreadName() const -> UTF8;

        std::shared_ptr<ThreadRecord> getThreadItem()
        {
            return threadStruct->thread;
        }

        static ThreadItem *makeThreadItem(const std::shared_ptr<ThreadListStruct> &threadStruct);
    };

    class ThreadItemWithIndicator : public ThreadItem
    {
        gui::Image *indicator = nullptr;

        void onDimensionChangedBottom(const BoundingBox &oldDim, const BoundingBox &newDim) override;

      public:
        ThreadItemWithIndicator(const UTF8 &indicatorName);
    };

    class ThreadItemNotRead : public ThreadItemWithIndicator
    {
        static constexpr auto indicatorName = "dot_12px_hard_alpha_W_M";

      public:
        ThreadItemNotRead();
    };

} /*namespace gui*/
