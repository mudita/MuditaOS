#include "PukLockBox.hpp"

#include "PinLockBaseWindow.hpp"
#include "application-desktop/widgets/PinLock.hpp"
#include "application-desktop/data/AppDesktopStyle.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/Image.hpp"
#include <i18/i18.hpp>
#include <Style.hpp>

namespace lock_style = style::window::pin_lock;

namespace gui
{
    void PukLockBox::popChar(uint32_t charNum)
    {
        LockWindow->pinLabels[charNum]->clear();
    }
    void PukLockBox::putChar(uint32_t charNum)
    {
        LockWindow->pinLabels[charNum]->setText("*");
    }
    void PukLockBox::buildLockBox(unsigned int pinSize)
    {
        LockWindow->buildImages("pin_lock", "pin_lock_info");
        LockWindow->buildInfoText(lock_style::info_text_h_puk);
        buildPinLabels(pinSize);
    }
    void PukLockBox::buildPinLabels(unsigned int pinSize)
    {
        // labels with stars for displaying entered digits
        const uint32_t pinLabelWidth = style::window_width - 2 * lock_style::pin_label_x;
        LockWindow->pinLabel         = new gui::Label(
            LockWindow, lock_style::pin_label_x, lock_style::pin_label_y, pinLabelWidth, lock_style::label_size);
        LockWindow->pinLabel->setEdges(RectangleEdge::Bottom);

        LockWindow->buildPinLabels(LockWindow->pinLabel, pinSize, lock_style::label_size);
        for (auto label : LockWindow->pinLabels) {
            label->setEdges(RectangleEdge::None);
        }
    }
    void PukLockBox::setVisibleStateEnterPin()
    {
        LockWindow->clearPinLabels();
        LockWindow->pinLabel->setVisible(true);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_blocked"));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_type_puk"));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(false, false, true);
    }
    void PukLockBox::setVisibleStateVerifiedPin()
    {
        LockWindow->clearPinLabels();
        LockWindow->pinLabel->setVisible(false);
        LockWindow->infoText->setVisible(false);
    }
    void PukLockBox::setVisibleStateInvalidPin()
    {
        LockWindow->clearPinLabels();
        LockWindow->pinLabel->setVisible(false);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_wrong_puk"));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_you_have"));
        LockWindow->infoText->addText(" ");
        LockWindow->infoText->addText(std::to_string(LockWindow->lock.getRemainingAttempts()));
        LockWindow->infoText->addText(" ");

        if (LockWindow->lock.getRemainingAttempts() > 1) {
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_attempt_left_plural"));
        }
        else {
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_attempt_left_singular"));
            LockWindow->infoText->addText("\n\n");
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_warning1"));
            LockWindow->infoText->addText("\n");
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_warning2"));
            LockWindow->infoText->addText("\n");
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_warning3"));
        }
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(true, true, false);
    }
    void PukLockBox::setVisibleStateBlocked()
    {
        LockWindow->pinLabel->setVisible(false);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_blocked_info1"));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_blocked_info2"));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(true, false, false);
    }

} // namespace gui
