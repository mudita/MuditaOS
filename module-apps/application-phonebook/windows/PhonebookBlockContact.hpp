#pragma once

#include "../data/PhonebookItemData.hpp"
#include "AppWindow.hpp"
#include "ContactRecord.hpp"
#include "Label.hpp"
#include "Text.hpp"
#include <memory>
#include <string>
#include <widgets/BoxLayout.hpp>

using namespace gui;

class PhonebookBlockContact : public AppWindow
{
  protected:
    /** labels */
    Label *titleLabel = nullptr;
    Text *confirmationLabel = nullptr;
    Label *noLabel = nullptr;
    Label *yesLabel = nullptr;
    Image *trashacnIcon = nullptr;
    /** currently displayed contact */
    std::shared_ptr<ContactRecord> contact = nullptr;

  public:
    PhonebookBlockContact(app::Application *app);
    virtual ~PhonebookBlockContact();
    void setContactData();
    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, SwitchData *data) override;
    bool handleSwitchData(SwitchData *data) override;
    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;
};
