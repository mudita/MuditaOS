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
    return style::window::calendar::item::checkBox::height;
}

gui::ListItem *CustomRepeatModel::getItem(gui::Order order)
{
    auto app = application;
    assert(app != nullptr);
    // maybe this should be different
    gui::CheckBoxItem *item = nullptr;
    switch (count) {
    case 7:
        item = new gui::CheckBoxItem(app, utils::localize.get(style::strings::common::Monday));
        break;
    case 6:
        item = new gui::CheckBoxItem(app, utils::localize.get(style::strings::common::Tuesday));
        break;
    case 5:
        item = new gui::CheckBoxItem(app, utils::localize.get(style::strings::common::Wednesday));
        break;
    case 4:
        item = new gui::CheckBoxItem(app, utils::localize.get(style::strings::common::Thursday));
        break;
    case 3:
        item = new gui::CheckBoxItem(app, utils::localize.get(style::strings::common::Friday));
        break;
    case 2:
        item = new gui::CheckBoxItem(app, utils::localize.get(style::strings::common::Saturday));
        break;
    case 1:
        item = new gui::CheckBoxItem(app, utils::localize.get(style::strings::common::Sunday));
        break;
    default:
        return item;
        break;
    }
    count--;
    return item;
}
