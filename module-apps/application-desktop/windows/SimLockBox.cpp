// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "SimLockBox.hpp"

#include "PinLockBaseWindow.hpp"
#include "application-desktop/widgets/PinLock.hpp"
#include "application-desktop/data/AppDesktopStyle.hpp"
#include "gui/widgets/Label.hpp"
#include <i18/i18.hpp>
#include <Style.hpp>

namespace lock_style = style::window::pin_lock;

namespace gui
{

    void SimLockBox::popChar(unsigned int charNum)
    {
        rebuildPinLabels(charNum);
    }
    void SimLockBox::putChar(unsigned int charNum)
    {
        rebuildPinLabels(++charNum);
    }

    void SimLockBox::buildLockBox(unsigned int pinSize)
    {
        LockWindow->buildImages("pin_lock", "pin_lock_info");
        LockWindow->buildInfoText(lock_style::info_text_h_sim);
        buildPinLabels(0);
    }
    void SimLockBox::buildPinLabels(unsigned int pinSize)
    {
        auto itemBuilder = []() -> Rect * {
            auto label = new gui::Image("dot_12px_hard_alpha_W_G");
            return label;
        };

        LockWindow->buildPinLabels(itemBuilder,
                                   pinSize,
                                   lock_style::pin_label_x,
                                   lock_style::pin_label_y,
                                   style::window_width - 2 * lock_style::pin_label_x);
        LockWindow->pinLabelsBox->setEdges(RectangleEdge::Bottom);
    }

    void SimLockBox::rebuildPinLabels(unsigned int pinSize)
    {
        LockWindow->pinLabelsBox->erase();
        buildPinLabels(pinSize);
    }

    void SimLockBox::setVisibleStateEnterPin()
    {
        LockWindow->pinLabelsBox->setVisible(true);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_to_unlock"));
        LockWindow->infoText->addText(" ");
        LockWindow->infoText->addText(LockWindow->lock.getLockInfo(PinLock::InfoName::LockName));
        LockWindow->infoText->addText(" ");
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_card"));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText("( ");
        LockWindow->infoText->addText(LockWindow->lock.getLockInfo(PinLock::InfoName::PhoneNum));
        LockWindow->infoText->addText(" )");
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_type_pin"));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(false, false, true);
    }
    void SimLockBox::setVisibleStateVerifiedPin()
    {
        LockWindow->infoText->clear();
        LockWindow->infoText->setVisible(false);
        LockWindow->pinLabelsBox->setVisible(false);
    }
    void SimLockBox::setVisibleStateInvalidPin()
    {
        LockWindow->pinLabelsBox->setVisible(false);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get(utils::localize.get("app_desktop_sim_wrong_pin")));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(utils::localize.get(utils::localize.get("app_desktop_sim_you_have")));
        LockWindow->infoText->addText(" ");
        LockWindow->infoText->addText(std::to_string(LockWindow->lock.getRemainingAttempts()));
        LockWindow->infoText->addText(" ");
        if (LockWindow->lock.getRemainingAttempts() > 1) {
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_attempt_left_plural"));
        }
        else {
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_attempt_left_singular"));
        }
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(true, true, true);
    }
    void SimLockBox::setVisibleStateBlocked()
    {
        LockWindow->pinLabelsBox->setVisible(false);
        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get(utils::localize.get("app_desktop_puk_lock1")));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(true, true, true);
    }
} // namespace gui
