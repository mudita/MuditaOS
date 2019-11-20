#pragma once

#include <memory>
#include <string>

#include "../data/PhonebookItemData.hpp"
#include "AppWindow.hpp"
#include "ContactRecord.hpp"
#include "Label.hpp"
#include "Style.hpp"
#include "Text.hpp"
#include <widgets/BoxLayout.hpp>

namespace gui
{

class PhonebookNewContact : public AppWindow
{
  public:
    enum class PageLabels
    {
        Name,
        Surname,
        Number_1,
        Number_2,
        Email
    };

  protected:
    struct Page
    {
        virtual void setVisibile(bool visible) = 0;
    };

    struct Page1 : public Page
    {
        gui::Label *labels[5] = {nullptr};
        gui::Text *text[5] = {nullptr};
        void setVisibile(bool visible)
        {
            for (uint32_t i = 0; i < 5; i++)
            {
                labels[i]->setVisible(visible);
                text[i]->setVisible(visible);
            }
        }
    };

    struct Page2 : public Page
    {
        gui::Label *speedValue = nullptr;
        gui::Label *speedDescription = nullptr;
        gui::Image *imageSpeed = nullptr;
        gui::Label *favValue = nullptr;
        gui::Label *favDescription = nullptr;
        gui::Image *imageFav = nullptr;
        gui::Image *imageTick = nullptr;

        gui::Label *addressLabel = nullptr;
        gui::Label *noteLabel = nullptr;

        gui::Text *text[2] = {nullptr};

        bool favSelected = false;

        void setVisibile(bool visible)
        {
            speedValue->setVisible(visible);
            speedDescription->setVisible(visible);
            imageSpeed->setVisible(visible);
            favValue->setVisible(visible);
            favDescription->setVisible(visible);
            imageFav->setVisible(visible);
            imageTick->setVisible(visible);

            addressLabel->setVisible(visible);
            noteLabel->setVisible(visible);
            text[0]->setVisible(visible);
            text[1]->setVisible(visible);
        }
    };

    Page1 page1;
    Page2 page2;
    uint32_t page = 0;

    std::shared_ptr<ContactRecord> contact = nullptr;

  public:
    PhonebookNewContact(app::Application *app);
    virtual ~PhonebookNewContact();
    void switchPage(uint32_t page);

    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, SwitchData *data) override;
    bool handleSwitchData(SwitchData *data) override;
    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;
    void setContactData();
};

} /* namespace gui */
