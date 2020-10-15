#include "KeypadLightWindow.hpp"

#include <application-settings-new/ApplicationSettings.hpp>
#include <i18/i18.hpp>

namespace gui
{

    KeypadLightWindow::KeypadLightWindow(app::Application *app) : BaseSettingsWindow(app, window::name::keypad_light)
    {
        buildInterface();
    }

    void KeypadLightWindow::buildInterface()
    {
        BaseSettingsWindow::buildInterface();
        setTitle(utils::localize.get("app_settings_display_keypad_light"));

        auto body =
            new VBox(nullptr,
                     0,
                     title->offset_h() + style::margins::big,
                     this->getWidth(),
                     this->getHeight() - style::margins::big - this->title->offset_h() - bottomBar->getHeight());
        body->setEdges(gui::RectangleEdge::None);

        auto toggleBoxes = [this](CheckBoxWithLabel &active) {
            for (CheckBoxWithLabel *box : boxes) {
                box->setChecked(false);
                if (&active == box) {
                    box->setChecked(!box->isChecked());
                }
            }
        };

        boxes.push_back(new CheckBoxWithLabel(
            body, 0, 0, utils::localize.get("app_settings_display_keypad_light_on"), toggleBoxes));
        boxes.push_back(new CheckBoxWithLabel(
            body, 0, 0, utils::localize.get("app_settings_display_keypad_light_active"), toggleBoxes));
        boxes.push_back(new CheckBoxWithLabel(
            body, 0, 0, utils::localize.get("app_settings_display_keypad_light_off"), toggleBoxes));

        addWidget(body);
        setFocusItem(body);
    }

} // namespace gui
