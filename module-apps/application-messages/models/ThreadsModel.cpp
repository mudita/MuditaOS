#include <module-services/service-db/messages/DBThreadMessage.hpp>
#include <module-services/service-db/api/DBServiceAPI.hpp>
#include "ThreadsModel.hpp"
#include "OptionWindow.hpp"
#include "application-messages/windows/ThreadWindowOptions.hpp"
#include "application-messages/widgets/ThreadItem.hpp"
#include "application-messages/data/SMSdata.hpp"

ThreadsModel::ThreadsModel(app::Application *app) : BaseThreadsRecordModel(app)
{}

unsigned int ThreadsModel::getMinimalItemHeight() const
{
    return style::window::messages::sms_thread_item_h;
}

gui::ListItem *ThreadsModel::getItem(gui::Order order)
{
    std::shared_ptr<ThreadRecord> thread = getRecord(order);

    if (thread.get() == nullptr) {
        return nullptr;
    }

    auto item = gui::ThreadItem::makeThreadItem(this, thread);
    item->setThreadItem(thread);
    item->activatedCallback = [=](gui::Item &item) {
        LOG_INFO("ThreadItem ActivatedCallback");
        if (application) {
            application->switchWindow(gui::name::window::thread_view, std::make_unique<SMSThreadData>(thread));
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
    auto query = std::make_unique<db::query::SMSThreadsGet>(offset, limit);
    query->setQueryListener(this);
    DBServiceAPI::GetQuery(getApplication(), db::Interface::Name::SMSThread, std::move(query));
}

auto ThreadsModel::handleQueryResponse(db::QueryResult *queryResult) -> bool
{
    auto msgResponse = dynamic_cast<db::query::SMSThreadsGetResults *>(queryResult);
    assert(msgResponse != nullptr);

    auto records_data = msgResponse->getResults();
    auto records      = std::make_unique<std::vector<ThreadRecord>>(records_data.begin(), records_data.end());

    return this->updateRecords(std::move(records));
}
