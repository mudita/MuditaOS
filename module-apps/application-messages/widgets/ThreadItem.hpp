// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "BaseThreadItem.hpp"
#include "ContactRecord.hpp"
#include "application-messages/models/ThreadsModel.hpp"

namespace gui
{

    class ThreadItem : public BaseThreadItem
    {
        ThreadsModel *model = nullptr;
        std::shared_ptr<ThreadRecord> thread;

        auto getContactRecord() -> ContactRecord;
        auto getNumberImportance(const ContactRecord &contact) -> std::optional<long int>;

        void setPreview();
        void setContactName(const ContactRecord &contactInfo, std::optional<long int> numberImportance = std::nullopt);

      public:
        explicit ThreadItem(ThreadsModel *model);

        void setThreadItem(std::shared_ptr<ThreadRecord> _thread);

        auto getThreadName() const -> UTF8;

        std::shared_ptr<ThreadRecord> getThreadItem()
        {
            return thread;
        }

        static ThreadItem *makeThreadItem(ThreadsModel *model, std::shared_ptr<ThreadRecord> thread);
    };

    class ThreadItemWithIndicator : public ThreadItem
    {
        gui::Image *indicator = nullptr;

        void onDimensionChangedBottom(const BoundingBox &oldDim, const BoundingBox &newDim) override;

      public:
        ThreadItemWithIndicator(ThreadsModel *model, const UTF8 &indicatorName);
    };

    class ThreadItemNotRead : public ThreadItemWithIndicator
    {
        static const inline UTF8 indicatorName = "dot_12px_hard_alpha_W_M";

      public:
        ThreadItemNotRead(ThreadsModel *model);
    };

} /*namespace gui*/
