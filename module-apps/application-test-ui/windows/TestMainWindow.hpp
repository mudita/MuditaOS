#pragma once

#include <functional>
#include <string>

#include "AppWindow.hpp"
#include "gui/widgets/BottomBar.hpp"
#include "gui/widgets/Image.hpp"
#include "gui/widgets/Label.hpp"
#include "gui/widgets/TopBar.hpp"
#include "gui/widgets/Window.hpp"

namespace gui {

class TestMainWindow : public AppWindow {
  protected:
    gui::Label *title;
    std::vector<gui::Label *> show_box;

    gui::Item *addOptionLabel(const UTF8 &text, std::function<bool(Item &)> activatedCallback);
    std::vector<UTF8> bottom_texts;

  public:
    TestMainWindow(app::Application *app);
    virtual ~TestMainWindow();

    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) override;

    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;
};

} /* namespace gui */
