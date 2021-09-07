// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include <AppWindow.hpp>

namespace gui
{
    class ListView;
    class MeditationModel;
} // namespace gui

namespace gui
{
    class MeditationListViewWindow : public AppWindow
    {
      protected:
        gui::ListView *list                    = nullptr;
        std::shared_ptr<MeditationModel> model = nullptr;

        void invalidate() noexcept;

      public:
        MeditationListViewWindow(app::Application *, std::string windowName);

        void onBeforeShow(ShowMode mode, SwitchData *data) override;
        void rebuild() override;
        void buildInterface() override;
        void destroyInterface() override;
    };

    class MeditationOptionsWindow : public MeditationListViewWindow
    {
      public:
        explicit MeditationOptionsWindow(app::Application *);
        void buildInterface() final;
    };

    class PreparationTimeWindow : public MeditationListViewWindow
    {
      public:
        explicit PreparationTimeWindow(app::Application *);
        void buildInterface() final;
    };
} // namespace gui
