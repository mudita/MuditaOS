// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "CheckBox.hpp"
#include "Style.hpp"
#include <InputEvent.hpp>
#include <i18n/i18n.hpp>

namespace gui
{

    CheckBox::CheckBox(Item *parent,
                       const uint32_t &x,
                       const uint32_t &y,
                       const uint32_t &w,
                       const uint32_t &h,
                       const std::function<void(const UTF8 &text)> &navBarTemporaryMode,
                       const std::function<void()> &navBarRestoreFromTemporaryMode,
                       nav_bar::Side navBarSide)
        : HBox(parent, x, y, w, h), navBarTemporaryMode(navBarTemporaryMode),
          navBarRestoreFromTemporaryMode(navBarRestoreFromTemporaryMode), navBarSide(navBarSide)

    {
        setEdges(RectangleEdge::Bottom);
        setAlignment(gui::Alignment(gui::Alignment::Horizontal::Center, gui::Alignment::Vertical::Center));

        image = new Image(this, "small_tick_32px_W_M");
        image->setVisible(false);

        applyCallbacks();
    }

    void CheckBox::applyCallbacks()
    {
        focusChangedCallback = [&](Item &item) {
            if (focus) {
                if (isChecked()) {
                    if (navBarTemporaryMode) {
                        navBarTemporaryMode(utils::translate("common_uncheck"));
                    }
                }
                else {
                    if (navBarTemporaryMode) {
                        navBarTemporaryMode(utils::translate("common_check"));
                    }
                }
            }
            else {
                setFocusItem(nullptr);
                if (navBarRestoreFromTemporaryMode) {
                    navBarRestoreFromTemporaryMode();
                }
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (!event.isShortRelease()) {
                return false;
            }
            if ((navBarSide == nav_bar::Side::Left && event.is(gui::KeyCode::KEY_LF)) ||
                (navBarSide == nav_bar::Side::Center && event.is(gui::KeyCode::KEY_ENTER))) {
                setCheck(!isChecked());
                if (isChecked()) {
                    if (navBarTemporaryMode) {
                        navBarTemporaryMode(utils::translate("common_uncheck"));
                    }
                }
                else {
                    if (navBarTemporaryMode) {
                        navBarTemporaryMode(utils::translate("common_check"));
                    }
                }
                return true;
            }

            return false;
        };
    }

    void CheckBox::setCheck(bool state)
    {
        checkState = state;
        image->setVisible(state);
        resizeItems();
    }

    bool CheckBox::isChecked()
    {
        return checkState;
    }

} /* namespace gui */
