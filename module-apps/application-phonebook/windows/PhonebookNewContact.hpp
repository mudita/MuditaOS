#pragma once

#include <memory>
#include <string>

#include "AppWindow.hpp"
#include <widgets/BoxLayout.hpp>

namespace gui {

class PhonebookNewContact : public AppWindow
{
  protected:
    std::vector<gui::Item> options;
    gui::Label *title;
    // TODO -> make this new class inheriting on vbox...
    struct Input {
        gui::Label name, input;
        Input(Input &&) = default;
        Input(gui::VBox* box, const UTF8 &text);
        virtual ~Input();
    };
    std::vector<Input *> el;
    gui::VBox *box;
    unsigned int pos;

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
