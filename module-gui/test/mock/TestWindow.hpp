#pragma once
#include <module-gui/gui/widgets/Window.hpp>

namespace gui
{
    class TestWindow : public Window
    {
      public:
        TestWindow(const std::string &name);
        void buildInterface() final;
        void destroyInterface() final;
        void rebuild() final;
    };
} // namespace gui
