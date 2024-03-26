// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OnBoardingOnOffWindow.hpp"
#include "ApplicationBellOnBoarding.hpp"
#include "widgets/BellBaseLayout.hpp"

#include <InputEvent.hpp>
#include <Image.hpp>
#include <ImageBox.hpp>
#include <TextFixedSize.hpp>
#include <Application.hpp>

#define SNVS_LPGPR3 (*(volatile uint32_t *)(0x400D4000 + 0x100 + 3 * sizeof(uint32_t)))
#define SNVS_LPGPR4 (*(volatile uint32_t *)(0x400D4000 + 0x100 + 4 * sizeof(uint32_t)))
#define SNVS_LPGPR5 (*(volatile uint32_t *)(0x400D4000 + 0x100 + 5 * sizeof(uint32_t)))

#define BOOT_LOOP_COUNTER      SNVS_LPGPR3
#define RESTART_COUNTER        SNVS_LPGPR4
#define IS_TEST_STARTED        SNVS_LPGPR5
#define TEST_STARTED_MAGIC_VAL 0xB055DEAD

namespace style::onboarding
{
    namespace row
    {
        constexpr auto marginInternal = 39;
        constexpr auto marginExternal = 137;
        constexpr auto minimumSizeX   = style::window_width;
        constexpr auto minimumSizeY   = 64;
    }; // namespace row

    namespace arrow
    {
        constexpr auto marginLeft = 24;
    }; // namespace arrow

    namespace text
    {
        constexpr auto marginLeft   = 15;
        constexpr auto minimumSizeX = 81;
        constexpr auto minimumSizeY = 62;
    } // namespace text
};    // namespace style::onboarding

namespace gui
{

    OnBoardingOnOffWindow::OnBoardingOnOffWindow(app::ApplicationCommon *app,
                                                 std::unique_ptr<AbstractPowerOffPresenter> presenter,
                                                 const std::string &name)
        : WindowWithTimer(app, name, displayDuration), presenter{std::move(presenter)}
    {
        LOG_ERROR(" BL test started reg val: 0x%08lX", static_cast<long>(IS_TEST_STARTED));
        LOG_ERROR(" BL test is already started if reg has val: 0x%08lX", static_cast<long>(TEST_STARTED_MAGIC_VAL));

        // This will only work once the first time you upload this SW package
        if (IS_TEST_STARTED != TEST_STARTED_MAGIC_VAL) {
            LOG_ERROR(" --- BOOT LOOP TEST STARTED ---");
            IS_TEST_STARTED   = TEST_STARTED_MAGIC_VAL;
            RESTART_COUNTER   = 0;
            BOOT_LOOP_COUNTER = 0;
        }

        RESTART_COUNTER++;
        BOOT_LOOP_COUNTER++; // Will decrement on correct turning off

        buildInterface();
    }

    void OnBoardingOnOffWindow::buildInterface()
    {
        AppWindow::buildInterface();
        buildLayout();
        registerCallbacks();
    }

    void OnBoardingOnOffWindow::buildLayout()
    {
        statusBar->setVisible(false);
        header->setTitleVisibility(false);
        navBar->setVisible(false);

        std::string correctBootsCount = std::to_string(RESTART_COUNTER);
        std::string bootLoopsCount    = std::to_string(SNVS_LPGPR3 - 1); // -1 b'case we won't count current startup
        std::string magicReg          = std::to_string(IS_TEST_STARTED);
        LOG_WARN(
            "--- BOOT LOOP TEST ---  Restarts:%s   BootLoops:%s", correctBootsCount.c_str(), bootLoopsCount.c_str());

        auto body = new BellBaseLayout(this, 0, 0, style::window_width, style::window_height, false);

        auto label1 = new Label(body->firstBox);
        label1->setMaximumSize(style::window_width, style::window_height);
        label1->setFont(style::window::font::largelight);
        label1->setEdges(RectangleEdge::Bottom);
        label1->setTextType(TextType::MultiLine);
        label1->activeItem = false;
        label1->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        label1->setText("Boot Loop Test\n"
                        "v. 2.5.9T1-officeTest");

        auto label2 = new Label(body->centerBox);
        label2->setMaximumSize(style::window_width, style::window_height);
        label2->setFont(style::window::font::verysmall);
        label2->setEdges(RectangleEdge::None);
        label2->setTextType(TextType::MultiLine);
        label2->activeItem = false;
        label2->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Center));
        label2->setText("This replaces the ON/OFF window.\n"
                        "Press FRONT LIGHT button to proceed onboarding\n or BACK button to turn off\n"
                        "(Reboot after 10 sec)");

        auto label3 = new Label(body->lastBox);
        label3->setMaximumSize(style::window_width, style::window_height);
        label3->setFont(style::window::font::mediumbigbold);
        label3->setEdges(RectangleEdge::Top);
        label3->setTextType(TextType::MultiLine);
        label3->activeItem = false;
        label3->setAlignment(Alignment(Alignment::Horizontal::Center, Alignment::Vertical::Top));
        label3->setText("Correct boot counter: " + correctBootsCount + "\n" +
                        "   Boot loop counter: " + bootLoopsCount);
        body->resize();

        //        auto mainVBox = new VBox(this, 0, 0, style::window_width, style::window_height);
        //        mainVBox->setEdges(RectangleEdge::None);
        //
        //        auto onHBox = new HBox(mainVBox, 0, 0, 0, 0);
        //        onHBox->setMinimumSize(style::onboarding::row::minimumSizeX, style::onboarding::row::minimumSizeY);
        //        onHBox->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center,
        //        gui::Alignment::Vertical::Center}); onHBox->setEdges(RectangleEdge::None); onHBox->setMargins({0,
        //        style::onboarding::row::marginExternal, 0, style::onboarding::row::marginInternal});
        //
        //        auto offHBox = new HBox(mainVBox, 0, 0, 0, 0);
        //        offHBox->setMinimumSize(style::onboarding::row::minimumSizeX, style::onboarding::row::minimumSizeY);
        //        offHBox->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center,
        //        gui::Alignment::Vertical::Center}); offHBox->setEdges(RectangleEdge::None); offHBox->setMargins({0,
        //        style::onboarding::row::marginInternal, 0, style::onboarding::row::marginExternal});
        //
        //        auto sunImg = new ImageBox(onHBox, 0, 0, 0, 0, new Image("button_icon_sun", ImageTypeSpecifier::W_G));
        //        sunImg->setMinimumSizeToFitImage();
        //        sunImg->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center,
        //        gui::Alignment::Vertical::Center});
        //
        //        auto onArrow = new ImageBox(onHBox, 0, 0, 0, 0, new Image("arrow_right_long",
        //        ImageTypeSpecifier::W_G)); onArrow->setMinimumSizeToFitImage();
        //        onArrow->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center,
        //        gui::Alignment::Vertical::Center}); onArrow->setMargins({style::onboarding::arrow::marginLeft, 0, 0,
        //        0});
        //
        //        auto onText = new TextFixedSize(onHBox, 0, 0, 200, 0);
        //        onText->setFont(style::window::font::verysmall);
        //        onText->setText("ON; R: " + correctBootsCount);
        //        onText->setMinimumSize(style::onboarding::text::minimumSizeX, style::onboarding::text::minimumSizeY);
        //        onText->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left,
        //        gui::Alignment::Vertical::Center}); onText->setEdges(RectangleEdge::None);
        //        onText->drawUnderline(false);
        //        onText->setMargins({style::onboarding::text::marginLeft, 0, 0, 0});
        //
        //        auto backImg = new ImageBox(offHBox, 0, 0, 0, 0, new Image("arrow_back", ImageTypeSpecifier::W_G));
        //        backImg->setMinimumSizeToFitImage();
        //        backImg->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center,
        //        gui::Alignment::Vertical::Center});
        //
        //        auto offArrow = new ImageBox(offHBox, 0, 0, 0, 0, new Image("arrow_right_long",
        //        ImageTypeSpecifier::W_G)); offArrow->setMinimumSizeToFitImage();
        //        offArrow->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Center,
        //        gui::Alignment::Vertical::Center}); offArrow->setMargins({style::onboarding::arrow::marginLeft, 0, 0,
        //        0});
        //
        //        auto offText = new TextFixedSize(offHBox, 0, 0, 200, 0);
        //        offText->setFont(style::window::font::verysmall);
        //        offText->setText("OFF; BL: " + bootLoopsCount);
        //        offText->setMinimumSize(style::onboarding::text::minimumSizeX, style::onboarding::text::minimumSizeY);
        //        offText->setAlignment(gui::Alignment{gui::Alignment::Horizontal::Left,
        //        gui::Alignment::Vertical::Center}); offText->setEdges(RectangleEdge::None);
        //        offText->drawUnderline(false);
        //        offText->setMargins({style::onboarding::text::marginLeft, 0, 0, 0});
        //
        //        onHBox->resizeItems();
        //        mainVBox->resizeItems();
        //        onHBox->resizeItems();
    }

    void OnBoardingOnOffWindow::registerCallbacks()
    {
        timerCallback = [this](Item &, sys::Timer &timer) {
            BOOT_LOOP_COUNTER--;
            LOG_ERROR("--- BOOT LOOP TEST --- auto reboot");
            presenter->reboot();
            return true;
        };
    }

    bool OnBoardingOnOffWindow::onInput(const InputEvent &inputEvent)
    {
        if (inputEvent.isShortRelease(KeyCode::KEY_LF)) {
            application->switchWindow(gui::window::name::onBoardingLanguageWindow);
            return true;
        }
        else if (inputEvent.isShortRelease(KeyCode::KEY_RF)) {
            BOOT_LOOP_COUNTER--;
            presenter->powerOff(sys::CloseReason::OnboardingPowerDown);
            return false;
        }
        return false;
    }
} // namespace gui
