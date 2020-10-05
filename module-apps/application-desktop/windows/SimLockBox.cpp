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

    void SimLockBox::popChar(uint32_t charNum)
    {
        LockWindow->pinLabels[charNum]->clear();
    }
    void SimLockBox::putChar(uint32_t charNum)
    {
        LockWindow->pinLabels[charNum]->setText("*");
    }

    void SimLockBox::buildLockBox(unsigned int pinSize)
    {
        LockWindow->buildImages("pin_lock", "pin_lock_info");
        LockWindow->buildInfoText(lock_style::info_text_h_sim);
        buildPinLabels(pinSize);
    }
    void SimLockBox::buildPinLabels(unsigned int pinSize)
    {
        // labels with stars for displaying entered digits
        const uint32_t pinLabelWidth = style::window_width - 2 * lock_style::pin_label_x;
        LockWindow->pinLabel         = new gui::Label(
            LockWindow, lock_style::pin_label_x, lock_style::pin_label_y, pinLabelWidth, lock_style::label_size);
        LockWindow->pinLabel->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM);

        LockWindow->buildPinLabels(LockWindow->pinLabel, pinSize, lock_style::label_size);
        for (auto label : LockWindow->pinLabels) {
            label->setEdges(RectangleEdgeFlags::GUI_RECT_EDGE_NO_EDGES);
        }
    }
    void SimLockBox::setVisibleStateEnterPin()
    {
        LockWindow->pinLabel->setVisible(true);

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
        LockWindow->clearPinLabels();
        LockWindow->infoText->clear();
        LockWindow->infoText->setVisible(false);
        LockWindow->pinLabel->setVisible(false);
    }
    void SimLockBox::setVisibleStateInvalidPin()
    {
        LockWindow->clearPinLabels();
        LockWindow->pinLabel->setVisible(false);

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
        LockWindow->pinLabel->setVisible(false);
        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get(utils::localize.get("app_desktop_puk_lock1")));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(true, true, true);
    }
} // namespace gui
