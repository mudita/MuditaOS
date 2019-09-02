#pragma once

#include <memory>
#include <string>

#include "AppWindow.hpp"
#include <widgets/BoxLayout.hpp>
#include "ContactInputBox.hpp"
#include "PhonebookData.hpp"

namespace gui {

class PhonebookOptions : public AppWindow
{
  protected:
      const int side_margin;
      gui::Label *title;
      std::vector<gui::Item *> options;
      std::unique_ptr<PhonebookContactData> data; // intermediate data for window switching

  public:
    PhonebookOptions(app::Application *app);
    virtual ~PhonebookOptions();

    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) override;
    bool handleSwitchData(SwitchData *data) override;

    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;

    gui::Item *addOptionLabel(const UTF8 &text, std::function<bool(Item &)> activatedCallback);
};

} /* namespace gui */
