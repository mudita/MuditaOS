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

    struct Page2 {
    	gui::Label* speedValue = nullptr;
    	gui::Label* speedDescription = nullptr;
    	gui::Image* imageSpeed = nullptr;
    	gui::Label* favValue = nullptr;
		gui::Label* favDescription = nullptr;
		gui::Image* imageFav = nullptr;

		gui::Label* addressLabel = nullptr;
		gui::Label* noteLabel = nullptr;

		gui::Text* text[2] = {nullptr};
    };

    Page1 page1;
    Page2 page2;
    uint32_t page = 0;
public:
    PhonebookNewContact(app::Application *app);
    virtual ~PhonebookNewContact();
    void switchPage( uint32_t page );

    // virtual methods
    bool onInput(const InputEvent &inputEvent) override;
    void onBeforeShow(ShowMode mode, uint32_t command, SwitchData *data) override;

    void rebuild() override;
    void buildInterface() override;
    void destroyInterface() override;
};

} /* namespace gui */
