#pragma once

#include <memory>
#include <string>

#include "AppWindow.hpp"
#include <widgets/BoxLayout.hpp>

namespace gui {

class PhonebookNewContact : public AppWindow
{
  protected:
    gui::Label *title = nullptr;
  public:
    PhonebookNewContact(app::Application *app);
    virtual ~PhonebookNewContact();

    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) override;

    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;
};

} /* namespace gui */
