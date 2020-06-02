#pragma once

#include "application-phonebook/ApplicationPhonebook.hpp"
#include "../data/PhonebookItemData.hpp"
#include "AppWindow.hpp"
#include "ContactRecord.hpp"
#include "Label.hpp"
#include "Text.hpp"
#include <Dialog.hpp>
#include <memory>
#include <string>
#include <widgets/BoxLayout.hpp>

using namespace gui;

class PhonebookDialog : public AppWindow
{
  public:
    PhonebookDialog(app::Application *app, const std::string &dialogName);
    virtual ~PhonebookDialog()
    {
        destroyInterface();
    }

    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, SwitchData *data) override
    {
        setFocusItem(noLabel);
    }
    bool handleSwitchData(SwitchData *data) override;
    void rebuild() override
    {
        destroyInterface();
        buildInterface();
    }
    void buildInterface() override;
    void destroyInterface() override
    {
        AppWindow::destroyInterface();
    }

    virtual void setContactData() = 0;

  protected:
    Text *confirmationLabel                = nullptr;
    Label *noLabel                         = nullptr;
    Label *yesLabel                        = nullptr;
    Image *icon                            = nullptr;
    std::shared_ptr<ContactRecord> contact = nullptr;
};

class DuplicatedContactDialogWindow : public DialogYesNo
{
  public:
    DuplicatedContactDialogWindow(app::Application *app);
    virtual ~DuplicatedContactDialogWindow() = default;
    bool handleSwitchData(SwitchData *data) override;

  private:
    static std::string updateText(const std::string &text, const ContactRecord &rec);
};

class PhonebookDuplicateSpeedDial : public PhonebookDialog
{
  protected:
    Label *dialValue = nullptr;

  public:
    PhonebookDuplicateSpeedDial(app::Application *app) : PhonebookDialog(app, gui::window::name::duplicated_contact)
    {}
    virtual ~PhonebookDuplicateSpeedDial() = default;
    void onBeforeShow(ShowMode mode, SwitchData *data);
    void setContactData();
};
