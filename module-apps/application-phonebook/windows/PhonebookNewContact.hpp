#pragma once

#include <string>
#include <memory>

#include "AppWindow.hpp"
#include <widgets/BoxLayout.hpp>

namespace gui {

class PhonebookNewContact : public AppWindow {
  protected:
      std::unique_ptr<gui::Label> title;
      // TODO -> make this new class inheriting on vbox...
    struct Input {
        std::unique_ptr<gui::Label> name;
        std::unique_ptr<gui::Label> input;
        void build(PhonebookNewContact *parent, const UTF8 &text);
    } name1; // , name2, nr1, nr2, email;
    std::unique_ptr<gui::VBox> box;

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
