#include "CustomRepeatModel.hpp"
#include "application-calendar/widgets/CheckBoxItem.hpp"
#include <ListView.hpp>
#include <Utils.hpp>

CustomRepeatModel::CustomRepeatModel(app::Application *app) : application(app)
{}

void CustomRepeatModel::requestRecords(const uint32_t offset, const uint32_t limit)
{}

unsigned int CustomRepeatModel::getMinimalItemHeight() const
{
    return 61; // style::window::calendar::item::customrepeat::height;
}

gui::ListItem *CustomRepeatModel::getItem(gui::Order order)
{
    auto app = application;
    // maybe this should be different
    gui::CheckBoxItem *item = nullptr;
    switch (count) {
    case 7:
        item = new gui::CheckBoxItem(
            utils::localize.get(style::strings::common::Monday),
            [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
            [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });
        break;
    case 6:
        item = new gui::CheckBoxItem(
            utils::localize.get(style::strings::common::Tuesday),
            [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
            [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });
        break;
    case 5:
        item = new gui::CheckBoxItem(
            utils::localize.get(style::strings::common::Wednesday),
            [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
            [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });
        break;
    case 4:
        item = new gui::CheckBoxItem(
            utils::localize.get(style::strings::common::Thursday),
            [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
            [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });
        break;
    case 3:
        item = new gui::CheckBoxItem(
            utils::localize.get(style::strings::common::Friday),
            [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
            [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });
        break;
    case 2:
        item = new gui::CheckBoxItem(
            utils::localize.get(style::strings::common::Saturday),
            [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
            [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });
        break;
    case 1:
        item = new gui::CheckBoxItem(
            utils::localize.get(style::strings::common::Sunday),
            [app](const UTF8 &text) { app->getCurrentWindow()->bottomBarTemporaryMode(text); },
            [app]() { app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode(); });
        break;
    default:
        return item;
        break;
    }
    count--;
    // item->activatedCallback = [=](gui::Item &item) {
    //    LOG_INFO("Check/Uncheck item");
    //    return true;
    //};
    // item->focusChangedCallback = [=](gui::Item &) {
    //    item->changeFont();
    //    return true;
    //};
    return item;
}
