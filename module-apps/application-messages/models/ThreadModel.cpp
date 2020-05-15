#include "ThreadModel.hpp"
#include "OptionWindow.hpp"
#include "application-messages/windows/OptionsWindow.hpp"
#include "../widgets/ThreadItem.hpp"
#include "application-messages/data/SMSdata.hpp"

ThreadModel::ThreadModel(app::Application *app) : BaseThreadRecordModel(app)
{}

gui::ListItem *ThreadModel::getItem(int index)
{
    std::shared_ptr<ThreadRecord> thread = getRecord(index);

    if (thread.get() == nullptr) {
        return nullptr;
    }

    auto item = new gui::ThreadItem(this);
    if (item != nullptr) {
        item->setThreadItem(thread);
        item->setID(index);
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
    return nullptr;
}
