#include "ThreadsModel.hpp"
#include "InputEvent.hpp"
#include "OptionWindow.hpp"
#include "application-messages/data/SMSdata.hpp"
#include "application-messages/data/MessagesStyle.hpp"
#include "application-messages/widgets/ThreadItem.hpp"
#include "application-messages/windows/ThreadWindowOptions.hpp"
#include <module-services/service-db/api/DBServiceAPI.hpp>

ThreadsModel::ThreadsModel(app::Application *app) : BaseThreadsRecordModel(app)
{}

auto ThreadsModel::getMinimalItemHeight() const -> unsigned int
{
    return style::messages::threadItem::sms_thread_item_h;
}

auto ThreadsModel::getItem(gui::Order order) -> gui::ListItem *
{
    std::shared_ptr<ThreadRecord> thread = getRecord(order);

    if (thread.get() == nullptr) {
        return nullptr;
    }

    auto item               = gui::ThreadItem::makeThreadItem(this, thread);
    item->activatedCallback = [this, thread](gui::Item &item) {
        LOG_INFO("ThreadItem ActivatedCallback");
        if (application) {
            const auto &threadItem = static_cast<gui::ThreadItem &>(item);
            application->switchWindow(gui::name::window::thread_view,
                                      std::make_unique<SMSThreadData>(thread, threadItem.getThreadName()));
        }
        else {
            LOG_ERROR("No application!");
        }
        return true;
    };

    item->inputCallback = [this, item](gui::Item &, const gui::InputEvent &event) {
        auto app = dynamic_cast<app::ApplicationMessages *>(application);
        assert(app);
        if (event.state != gui::InputEvent::State::keyReleasedShort) {
            return false;
        }
        if (event.keyCode == gui::KeyCode::KEY_LF) {
            app->windowOptions->clearOptions();
            app->windowOptions->addOptions(threadWindowOptions(app, item->getThreadItem().get()));
            app->switchWindow(app->windowOptions->getName(), nullptr);
        }
        return false;
    };
    return item;
}

void ThreadsModel::requestRecords(uint32_t offset, uint32_t limit)
{
    auto query = std::make_unique<db::query::ThreadsGet>(offset, limit);
    query->setQueryListener(
        db::QueryCallback::fromFunction([this](auto response) { return handleQueryResponse(response); }));
    DBServiceAPI::GetQuery(getApplication(), db::Interface::Name::SMSThread, std::move(query));
}

auto ThreadsModel::handleQueryResponse(db::QueryResult *queryResult) -> bool
{
    auto msgResponse = dynamic_cast<db::query::ThreadsGetResults *>(queryResult);
    assert(msgResponse != nullptr);

    auto records_data = msgResponse->getResults();
    auto records      = std::make_unique<std::vector<ThreadRecord>>(records_data.begin(), records_data.end());
    return this->updateRecords(std::move(records));
}
