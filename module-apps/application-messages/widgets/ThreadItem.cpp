// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ThreadItem.hpp"
#include "time/time_conversion.hpp"

#include "service-db/api/DBServiceAPI.hpp"

#include <Style.hpp>
#include "application-messages/data/MessagesStyle.hpp"

namespace gui
{
    ThreadItem *ThreadItem::makeThreadItem(const std::shared_ptr<ThreadListStruct> &threadStruct)
    {
        ThreadItem *threadItem = threadStruct->thread->isUnread() ? new ThreadItemNotRead() : new ThreadItem();
        threadItem->setThreadItem(threadStruct);
        return threadItem;
    }

    auto ThreadItem::getThreadName() const -> UTF8
    {
        std::ostringstream name;
        name << contact->getText();
        if (const auto &importance = numberImportance->getText(); !importance.empty()) {
            name << ' ' << importance;
        }
        return name.str();
    }

    void ThreadItem::setPreview()
    {
        UTF8 prefix;
        switch (threadStruct->thread->type) {
        case SMSType::DRAFT:
            prefix = utils::localize.get("app_messages_thread_draft");
            break;
        case SMSType::FAILED:
            prefix = utils::localize.get("app_messages_thread_not_sent");
            break;
        case SMSType::OUTBOX:
        case SMSType::QUEUED:
            prefix = utils::localize.get("app_messages_thread_you");
            break;
        default:
            break;
        }
        preview->setText(prefix + threadStruct->thread->snippet);
    }

    void ThreadItem::setThreadItem(std::shared_ptr<ThreadListStruct> _threadStruct)
    {
        threadStruct = std::move(_threadStruct);

        setContactName(getNumberImportance());
        timestamp->setText(utils::time::DateTime(threadStruct->thread->date));
        setPreview();
    }

    auto ThreadItem::getNumberImportance() -> std::optional<long>
    {
        if (threadStruct->contact->numbers.size() < 2) {
            // At least two phone numbers are needed to indicate a number importance.
            return std::nullopt;
        }

        const auto numberRec     = threadStruct->number;
        const auto &phoneNumbers = threadStruct->contact->numbers;

        const auto it = std::find_if(phoneNumbers.begin(), phoneNumbers.end(), [&numberRec](const auto &phoneNumber) {
            return phoneNumber.number == *numberRec;
        });

        if (it == phoneNumbers.end()) {
            return std::nullopt;
        }
        return std::distance(phoneNumbers.begin(), it) + 1;
    }

    void ThreadItem::setContactName(std::optional<long int> numberImportance)
    {
        contact->setText(threadStruct->contact->getFormattedName());
        if (numberImportance.has_value()) {
            displayNumberImportance(numberImportance.value());
        }
    }

    ThreadItemWithIndicator::ThreadItemWithIndicator(const UTF8 &indicatorName) : ThreadItem()
    {
        indicator = new gui::Image(this, 0, 0, indicatorName);
    }

    void ThreadItemWithIndicator::onDimensionChangedBottom(const BoundingBox & /*oldDim*/, const BoundingBox &newDim)
    {
        namespace msgStyle = style::messages::threadItem;

        const auto indicatorWidth = indicator->getSize(gui::Axis::X) + 6;
        preview->setPosition(msgStyle::leftMargin + indicatorWidth, newDim.h / 2);
        preview->setSize(newDim.w - msgStyle::previewWidthOffset - indicatorWidth,
                         newDim.h / 2 - msgStyle::bottomMargin);

        indicator->setPosition(msgStyle::leftMargin,
                               newDim.h / 2 + preview->getSize(gui::Axis::Y) / 2 -
                                   indicator->getSize(gui::Axis::Y) / 2); // align with text
    }

    ThreadItemNotRead::ThreadItemNotRead() : ThreadItemWithIndicator(ThreadItemNotRead::indicatorName)
    {}
} /*namespace gui*/
