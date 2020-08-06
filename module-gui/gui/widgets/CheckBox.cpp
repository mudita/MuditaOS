#include "CheckBox.hpp"
#include "Style.hpp"
#include "Utils.hpp"

namespace gui
{

    CheckBox::CheckBox(Item *parent,
                       const uint32_t &x,
                       const uint32_t &y,
                       const uint32_t &w,
                       const uint32_t &h,
                       std::function<void(const UTF8 &)> bottomBarTemporaryMode,
                       std::function<void()> bottomBarRestoreFromTemporaryMode)
        : ImageBox(parent, x, y, w, h, image = new Image("small_tick")),
          bottomBarTemporaryMode(std::move(bottomBarTemporaryMode)),
          bottomBarRestoreFromTemporaryMode(std::move(bottomBarRestoreFromTemporaryMode))
    {
        setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

        focusChangedCallback = [&](Item &item) {
            if (focus) {
                LOG_DEBUG("FOCUS");
                setFocusItem(image);
                if (image->visible) {
                    // bottomBarTemporaryMode(utils::localize.get("app_calendar_uncheck"), BottomBar::Side::LEFT,
                    // false);
                }
                else {
                    // bottomBarTemporaryMode(utils::localize.get("app_calendar_check"), BottomBar::Side::LEFT, false);
                }
            }
            else {
                LOG_DEBUG("NOT FOCUS");
                setFocusItem(nullptr);
                // bottomBarRestoreFromTemporaryMode();
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            LOG_DEBUG("INPUT CALLBACK");
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (event.keyCode == gui::KeyCode::KEY_ENTER) {
                LOG_DEBUG("KEY ENTER");
                // image->setVisible(!image->visible);
                /*if (image->visible) {
                    bottomBarTemporaryMode(utils::localize.get("app_calendar_uncheck"));
                }
                else {
                    bottomBarTemporaryMode(utils::localize.get("app_calendar_check"));
                }*/
                return true;
            }
            return false;
        };
    }

} /* namespace gui */
