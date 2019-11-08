/*
 * @file CalllogItem.hpp
 * @author Aleksander Rudnik (aleksander.rudnik@mudita.com)
 * @date 23.09.2019
 * @brief Call Log Item
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */
#pragma once

#include "Label.hpp"
#include "Image.hpp"
#include "ListItem.hpp"
#include "../data/CallLogInternals.hpp"
#include "../CalllogModel.hpp"

namespace gui {

/*
 * @brief Widget used to display information about calllog entry in the calllog list view.
 */
class CalllogItem: public ListItem {
	CalllogModel* model = nullptr;
	//pointer to the calls record
	std::shared_ptr<CalllogRecord> call = nullptr;
	//this is timestamp in the mode defined in settings
	gui::Label* timestamp = nullptr;
	gui::Image* imageCallType[calllog::CallLogCallType::NUM_OF_CALL_TYPES] = {nullptr,nullptr,nullptr};
	gui::Label* text = nullptr;
	//flag that defines if time should be displayed in 24h mode
	bool mode24H = false;

	public:
		CalllogItem( CalllogModel* model, bool mode24H );
		virtual ~CalllogItem();
		//sets copy of alarm's
		void setCall( std::shared_ptr<CalllogRecord>&  );

		//virtual methods from Item
		bool onDimensionChanged( const BoundingBox& oldDim, const BoundingBox& newDim) override;
};

} /* namespace gui */

