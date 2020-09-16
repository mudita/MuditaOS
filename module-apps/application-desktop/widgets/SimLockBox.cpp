#include "SimLockBox.hpp"

#include "../windows/PinLockBaseWindow.hpp"
#include "../widgets/PinLock.hpp"
#include "../data/AppDesktopStyle.hpp"
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
    void SimLockBox::buildPinLabels(uint32_t pinSize)
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
        LockWindow->clearPinLabels();
        LockWindow->pinLabel->setVisible(true);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_lock1"));
        LockWindow->infoText->addText(" ");
        LockWindow->infoText->addText(LockWindow->Lock->getLockInfo(LockInfo::InfoName::LockName));
        LockWindow->infoText->addText(" ");
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_lock2"));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText("( ");
        LockWindow->infoText->addText(LockWindow->Lock->getLockInfo(LockInfo::InfoName::PhoneNum));
        LockWindow->infoText->addText(" )");
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_lock3"));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(true, false);
        LockWindow->setBottomBarWidgetsActive(false, false, true);
    }
    void SimLockBox::setVisibleStateVerifiedPin()
    {
        LockWindow->infoText->clear();
        LockWindow->infoText->setVisible(false);
        LockWindow->pinLabel->setVisible(false);
    }
    void SimLockBox::setVisibleStateInvalidPin(uint32_t remainingAttempts)
    {
        LockWindow->pinLabel->setVisible(false);

        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get(utils::localize.get("app_desktop_sim_info1")));
        LockWindow->infoText->addText("\n");
        LockWindow->infoText->addText(utils::localize.get(utils::localize.get("app_desktop_sim_info2")));
        LockWindow->infoText->addText(" ");
        LockWindow->infoText->addText(utils::localize.get(std::to_string(remainingAttempts)));
        LockWindow->infoText->addText(" ");
        if (remainingAttempts > 1) {
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_info3"));
        }
        else {
            LockWindow->infoText->addText(utils::localize.get("app_desktop_sim_info4"));
        }
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(true, true, true);
    }
    void SimLockBox::setVisibleStatePhoneBlocked()
    {
        LockWindow->pinLabel->setVisible(false);
        LockWindow->infoText->clear();
        LockWindow->infoText->addText(utils::localize.get(utils::localize.get("app_desktop_puk_lock1")));
        LockWindow->infoText->setVisible(true);

        LockWindow->setImagesVisible(false, true);
        LockWindow->setBottomBarWidgetsActive(true, true, true);
    }
} // namespace gui
