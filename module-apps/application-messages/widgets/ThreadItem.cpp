#include "ThreadItem.hpp"
#include "time/time_conversion.hpp"

#include "service-db/api/DBServiceAPI.hpp"

#include <Style.hpp>
#include "application-messages/data/MessagesStyle.hpp"

namespace gui
{
    ThreadItem *ThreadItem::makeThreadItem(ThreadsModel *model, std::shared_ptr<ThreadRecord> thread)
    {
        ThreadItem *threadItem = thread->isUnread() ? new ThreadItemNotRead(model) : new ThreadItem(model);
        threadItem->setThreadItem(thread);
        return threadItem;
    }

    ThreadItem::ThreadItem(ThreadsModel *model)
    {
        this->model = model;
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
        switch (thread->type) {
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
        preview->setText(prefix + thread->snippet);
    }

    void ThreadItem::setThreadItem(std::shared_ptr<ThreadRecord> _thread)
    {
        thread = std::move(_thread);
        if (model != nullptr) {
            const auto &contact = getContactRecord();
            setContactName(contact, getNumberImportance(contact));
        }

        timestamp->setText(utils::time::DateTime(thread->date));
        setPreview();
    }

    auto ThreadItem::getContactRecord() -> ContactRecord
    {
        auto contactRec = DBServiceAPI::ContactGetByIDWithTemporary(model->getApplication(), thread->contactID);
        return contactRec->front();
    }

    auto ThreadItem::getNumberImportance(const ContactRecord &contact) -> std::optional<long>
    {
        if (contact.numbers.size() < 2) {
            // At least two phone numbers are needed to indicate a number importance.
            return std::nullopt;
        }

        const auto numberRec     = DBServiceAPI::GetNumberById(model->getApplication(), thread->numberID);
        const auto &phoneNumbers = contact.numbers;

        const auto it = std::find_if(phoneNumbers.begin(), phoneNumbers.end(), [&numberRec](const auto &phoneNumber) {
            return phoneNumber.number == *numberRec;
        });

        if (it == phoneNumbers.end()) {
            return std::nullopt;
        }
        return std::distance(phoneNumbers.begin(), it) + 1;
    }

    void ThreadItem::setContactName(const ContactRecord &contactInfo, std::optional<long int> numberImportance)
    {
        contact->setText(contactInfo.getFormattedName());
        if (numberImportance.has_value()) {
            displayNumberImportance(numberImportance.value());
        }
    }

    ThreadItemWithIndicator::ThreadItemWithIndicator(ThreadsModel *model, const UTF8 &indicatorName) : ThreadItem(model)
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

    ThreadItemNotRead::ThreadItemNotRead(ThreadsModel *model)
        : ThreadItemWithIndicator(model, ThreadItemNotRead::indicatorName)
    {}
} /*namespace gui*/
