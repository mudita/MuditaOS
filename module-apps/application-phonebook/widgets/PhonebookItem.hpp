/*
 * @file PhonebookItem.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 10 wrz 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_PHONEBOOK_WIDGETS_PHONEBOOKITEM_HPP_
#define MODULE_APPS_APPLICATION_PHONEBOOK_WIDGETS_PHONEBOOKITEM_HPP_


#include "Label.hpp"
#include "ListItem.hpp"
#include "../PhonebookModel.hpp"

namespace gui {

/*
 *
 */
class PhonebookItem: public ListItem {
	PhonebookModel* model = nullptr;
	//pointer to the contact record
	std::shared_ptr<ContactRecord> contact = nullptr;
	//this is contact's name and surname or "favourites" or "first letter of surname
	gui::Label* value = nullptr;

public:
	PhonebookItem( PhonebookModel* model );
	virtual ~PhonebookItem();
	//sets copy of contact
	void setContact( std::shared_ptr<ContactRecord> note );
	void setValue( UTF8 text );

	//returns contact's structure
	std::shared_ptr<ContactRecord> getContact();
	//virtual methods from Item
	bool onDimensionChanged( const BoundingBox& oldDim, const BoundingBox& newDim) override;
	bool onActivated( void* data ) override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_PHONEBOOK_WIDGETS_PHONEBOOKITEM_HPP_ */
