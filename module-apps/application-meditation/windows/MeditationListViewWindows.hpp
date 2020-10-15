#pragma once

#include "AppWindow.hpp"

namespace gui
{
    class ListView;
    class MeditationProvider;
} // namespace gui

namespace gui
{
    class MeditationListViewWindow : public AppWindow
    {
      protected:
        gui::ListView *list                          = nullptr;
        std::shared_ptr<MeditationProvider> provider = nullptr;

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
