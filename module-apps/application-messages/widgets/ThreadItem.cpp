#include "ThreadItem.hpp"
#include "time/time_conversion.hpp"

#include "service-db/api/DBServiceAPI.hpp"

#include <Style.hpp>
#include "../MessagesStyle.hpp"

namespace gui
{

    ThreadItem::ThreadItem(ThreadModel *model)
    {
        this->model = model;
    }

    void ThreadItem::setThreadItem(std::shared_ptr<ThreadRecord> &thread)
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
        namespace msgStyle = style::messages::threadItem;

        contact->setPosition(msgStyle::leftMargin, msgStyle::topMargin);
        contact->setSize(newDim.w - msgStyle::cotactWidthOffset, newDim.h / 2 - msgStyle::topMargin);

        timestamp->setPosition(newDim.w - msgStyle::timestampWidth, msgStyle::topMargin);
        timestamp->setSize(msgStyle::timestampWidth, newDim.h / 2 - msgStyle::topMargin);

        preview->setPosition(msgStyle::leftMargin + msgStyle::notSentIconWidth, newDim.h / 2);
        preview->setSize(newDim.w - msgStyle::previewWidthOffset - indicator->getSize(gui::Axis::X),
                         newDim.h / 2 - msgStyle::bottomMargin);

        indicator->setPosition(msgStyle::leftMargin, newDim.h / 2);

        return true;
    }

    ThreadItemNotSent::ThreadItemNotSent(ThreadModel *model)
        : ThreadItemWithIndicator(model, ThreadItemNotSent::indicatorName)
    {}

    ThreadItemNotRead::ThreadItemNotRead(ThreadModel *model)
        : ThreadItemWithIndicator(model, ThreadItemNotRead::indicatorName)
    {}
} /*namespace gui*/
