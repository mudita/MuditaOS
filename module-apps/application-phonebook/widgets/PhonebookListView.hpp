/*
 * @file PhonebookListView.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 11 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_PHONEBOOK_WIDGETS_PHONEBOOKLISTVIEW_HPP_
#define MODULE_APPS_APPLICATION_PHONEBOOK_WIDGETS_PHONEBOOKLISTVIEW_HPP_

#include "Application.hpp"
#include "ListView.hpp"

namespace gui {

/*
 *
 */
class PhonebookListView: public ListView {
	/**
	 * Pointer to application that owns the model
	 */
	app::Application* application = nullptr;
public:
	PhonebookListView();
	PhonebookListView( Item* parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h );
	virtual ~PhonebookListView();
	void setApplication( app::Application* app );


	bool onInput( const InputEvent& inputEvent ) override;
	void updatePageItems() override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_PHONEBOOK_WIDGETS_PHONEBOOKLISTVIEW_HPP_ */
