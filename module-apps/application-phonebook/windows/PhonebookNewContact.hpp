#pragma once

#include <memory>
#include <string>

#include "AppWindow.hpp"
#include "Label.hpp"
#include "Text.hpp"
#include <widgets/BoxLayout.hpp>

namespace gui {

class PhonebookNewContact : public AppWindow
{
public:
	enum class PageLabels{
		Name,
		Surname,
		Number_1,
		Number_2,
		Email
	};

protected:
    gui::Label *title = nullptr;

    struct Page1 {
		gui::Label* labels[5] = {nullptr};
		gui::Text* text[5] = {nullptr};
    };

    Page1 page1;
    uint32_t page = 0;
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
