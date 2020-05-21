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

} /*namespace gui*/
