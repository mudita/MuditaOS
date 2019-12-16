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

class PhonebookOptions : public AppWindow
{
  public:
    PhonebookOptions(app::Application *app);
    virtual ~PhonebookOptions();
    void buildInterface();
    void onBeforeShow(ShowMode mode, SwitchData *data) override;
    bool onInput(const InputEvent &inputEvent) override;
    bool handleSwitchData(SwitchData *data) override;

  protected:
    Label *topSeparatorLabel = nullptr;
    std::vector<gui::Item *> options;
    std::shared_ptr<ContactRecord> contact = nullptr;
    Item *addOptionLabel(const std::string &text, bool hasSubOptions, std::function<bool(Item &)> activatedCallback);
};
