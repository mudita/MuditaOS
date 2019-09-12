/*
 * @file NotesModel.hpp
 * @author Robert Borzecki (robert.borzecki@mudita.com)
 * @date 8 sie 2019
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#ifndef MODULE_APPS_APPLICATION_NOTES_NOTESMODEL_HPP_
#define MODULE_APPS_APPLICATION_NOTES_NOTESMODEL_HPP_

#include <vector>

#include "Interface/NotesRecord.hpp"
#include "DatabaseModel.hpp"
#include "NotesRecord.hpp"
#include "Application.hpp"
#include "ListItemProvider.hpp"

/*
 *
 */
class NotesModel: public app::DatabaseModel<NotesRecord>, public gui::ListItemProvider {
public:
	NotesModel( app::Application* app );
	virtual ~NotesModel();

	//virtual methods
	void requestRecordsCount() override;
	bool updateRecords( std::unique_ptr<std::vector<NotesRecord>> records, const uint32_t offset, const uint32_t limit, uint32_t count ) override;
	void requestRecords( const uint32_t offset, const uint32_t limit ) override;

	//virtual methods for ListViewProvider
	gui::ListItem* getItem( int index, int firstElement, int prevElement, uint32_t count ) override;
	int getItemCount();
};

#endif /* MODULE_APPS_APPLICATION_NOTES_NOTESMODEL_HPP_ */
