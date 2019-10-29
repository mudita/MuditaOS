/*
 * @file CalllogModel.hpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log Model
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include <vector>

#include "Interface/CalllogRecord.hpp"
#include "DatabaseModel.hpp"
#include "CalllogRecord.hpp"
#include "Application.hpp"
#include "ListItemProvider.hpp"

/*
 *
 */
class CalllogModel: public app::DatabaseModel<CalllogRecord>, public gui::ListItemProvider {
public:
	CalllogModel() = delete;
	CalllogModel( app::Application* app );
	virtual ~CalllogModel() = default;

	//virtual methods
	void requestRecordsCount() override;
	bool updateRecords( std::unique_ptr<std::vector<CalllogRecord>> records, const uint32_t offset, const uint32_t limit, uint32_t count ) override;
	void requestRecords( const uint32_t offset, const uint32_t limit ) override;

	//virtual methods for ListViewProvider
	gui::ListItem* getItem( int index, int firstElement, int prevElement, uint32_t count, int remaining, bool topDown ) override;
	int getItemCount() const override { return recordsCount; };
};
