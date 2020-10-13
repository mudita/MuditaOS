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

        virtual void invalidate() noexcept;

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
        MeditationOptionsWindow(app::Application *);
        void buildInterface() final;
    };

    class PreparationTimeWindow : public MeditationListViewWindow
    {
      public:
        PreparationTimeWindow(app::Application *);
        void buildInterface() final;
    };
} // namespace gui
