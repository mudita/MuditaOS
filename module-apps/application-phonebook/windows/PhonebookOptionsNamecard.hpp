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

class PhonebookOptionsNamecard : public AppWindow
{
  public:
    PhonebookOptionsNamecard(app::Application *app);
    virtual ~PhonebookOptionsNamecard();
    void buildInterface();
    void onBeforeShow(ShowMode mode, SwitchData *data) override;
    bool onInput(const InputEvent &inputEvent) override;
    bool handleSwitchData(SwitchData *data) override;
    void sendViaSms();
    void sendViaBluetooth();
    const std::string formatVCARD();

  protected:
    Text *dbg = nullptr;
    std::vector<gui::Item *> options;
    std::shared_ptr<ContactRecord> contact = nullptr;
    Item *addOptionLabel(const std::string &text, bool hasSubOptions, std::function<bool(Item &)> activatedCallback);
};
