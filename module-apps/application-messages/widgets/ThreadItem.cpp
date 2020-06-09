#include "ThreadItem.hpp"
#include "time/time_conversion.hpp"

#include "service-db/api/DBServiceAPI.hpp"

#include <Style.hpp>
#include "../MessagesStyle.hpp"

namespace gui
{
    ThreadItem *ThreadItem::makeThreadItem(ThreadModel *model, std::shared_ptr<ThreadRecord> thread)
    {
        ThreadItem *threadItem = nullptr;
        threadItem             = !thread->msgRead ? new ThreadItemNotRead(model) : new ThreadItem(model);

        threadItem->setThreadItem(thread);

        return threadItem;
    }

    ThreadItem::ThreadItem(ThreadModel *model)
    {
        this->model = model;
    }

    void ThreadItem::setThreadItem(std::shared_ptr<ThreadRecord> thread)
    {
        this->thread = thread;

        if (model != nullptr) {
            auto contactRec = DBServiceAPI::ContactGetByID(model->getApplication(), thread->contactID);
            auto cont       = contactRec->front();
            contact->setText(cont.getFormattedName());
        }

        timestamp->setText(utils::time::DateTime(thread->date));

        preview->setText(thread->snippet);
    }

    ThreadItemWithIndicator::ThreadItemWithIndicator(ThreadModel *model, const UTF8 &indicatorName) : ThreadItem(model)
    {
        indicator = new gui::Image(this, 0, 0, indicatorName);
    }

    bool ThreadItemWithIndicator::onDimensionChanged(const BoundingBox & /*oldDim*/, const BoundingBox &newDim)
    {
        LOG_DEBUG("onDimensionChanged this %lx", (uint64_t)this);
        namespace msgStyle = style::messages::threadItem;

        contact->setPosition(msgStyle::leftMargin, msgStyle::topMargin);
        contact->setSize(newDim.w - msgStyle::cotactWidthOffset, newDim.h / 2 - msgStyle::topMargin);

        timestamp->setPosition(newDim.w - msgStyle::timestampWidth, msgStyle::topMargin);
        timestamp->setSize(msgStyle::timestampWidth, newDim.h / 2 - msgStyle::topMargin);

        preview->setPosition(msgStyle::leftMargin + msgStyle::notSentIconWidth, newDim.h / 2);
        preview->setSize(newDim.w - msgStyle::previewWidthOffset - msgStyle::notSentIconWidth,
                         newDim.h / 2 - msgStyle::bottomMargin);

        LOG_FATAL("newDim.h2 %u, preview Y %u, indicator Y%u, Y %u",
                  newDim.h / 2,
                  preview->getSize(gui::Axis::Y),
                  indicator->getSize(gui::Axis::Y),
                  newDim.h / 2 + preview->getSize(gui::Axis::Y) - indicator->getSize(gui::Axis::Y));
        indicator->setPosition(msgStyle::leftMargin,
                               newDim.h / 2 + preview->getSize(gui::Axis::Y) / 2 -
                                   indicator->getSize(gui::Axis::Y) / 2);

        return true;
    }

    ThreadItemNotRead::ThreadItemNotRead(ThreadModel *model)
        : ThreadItemWithIndicator(model, ThreadItemNotRead::indicatorName)
    {}
} /*namespace gui*/
