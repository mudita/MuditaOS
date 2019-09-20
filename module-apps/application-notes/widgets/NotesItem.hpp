/*
 * @file NotesItem.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 12 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_NOTES_WIDGETS_NOTESITEM_HPP_
#define MODULE_APPS_APPLICATION_NOTES_WIDGETS_NOTESITEM_HPP_


#include "Label.hpp"
#include "ListItem.hpp"
#include "../NotesModel.hpp"

namespace gui {

/*
 * @brief Widget used to display information about note in the notes list view.
 */
class NotesItem: public ListItem {

	NotesModel* model = nullptr;
	//pointer to the notes record
	std::shared_ptr<NotesRecord> note = nullptr;
	//this is hour in the mode defined in settings
	gui::Label* hour = nullptr;
	gui::Label* title = nullptr;
	gui::Label* snippet = nullptr;
	//flag that defines if time should be displayed in 24h mode
	bool mode24H = false;

	public:
		NotesItem( NotesModel* model, bool mode24H );
		virtual ~NotesItem();
		//sets copy of alarm's
		void setNote( std::shared_ptr<NotesRecord>& note );

		//returns alarm structure
		std::shared_ptr<NotesRecord> getAlarm();
		//virtual methods from Item
		bool onDimensionChanged( const BoundingBox& oldDim, const BoundingBox& newDim) override;
		bool onActivated( void* data ) override;
};

} /* namespace gui */

#endif /* MODULE_APPS_APPLICATION_NOTES_WIDGETS_NOTESITEM_HPP_ */
