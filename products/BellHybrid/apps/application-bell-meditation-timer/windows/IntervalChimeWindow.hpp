// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <Application.hpp>
#include <InputEvent.hpp>
#include <Utils.hpp>
#include <Style.hpp>
#include <BoxLayout.hpp>
#include <Label.hpp>
#include <Image.hpp>

#include "MeditationWindow.hpp"

namespace gui
{
    namespace name::window
    {
        inline constexpr auto interval_chime = "Interval chime";
    }

    enum IntervalType
    {
        INTERVAL_NONE = 0,
        INTERVAL_1,
        INTERVAL_2,
        INTERVAL_5,
        INTERVAL_10,
        INTERVAL_15
    };

    class IntervalChimeWindow : public MeditationWindow
    {
      public:
        explicit IntervalChimeWindow(app::Application *app);

        // virtual methods
        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
        bool onInput(const gui::InputEvent &inputEvent) override;

      private:
        uint32_t intervalType     = 0;
        gui::Image *previousImage = nullptr;
        gui::Image *nextImage     = nullptr;
        gui::Label *title         = nullptr;
        gui::Label *text          = nullptr;
        gui::Label *minute        = nullptr;

        void previousInterval();
        void nextInterval();
        void updateDisplay();
        std::string getIntervalString();
        uint32_t intervalToSecs();
    };
} // namespace gui
