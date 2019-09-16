/*
 * ListItemProvider.hpp
 *
 *  Created on: 11 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_LISTITEMPROVIDER_HPP_
#define MIDDLEWARES_GUI_WIDGETS_LISTITEMPROVIDER_HPP_

#include "ListItem.hpp"
#include "ListView.hpp"

namespace gui {

class ListView;

class ListItemProvider {
protected:
	ListView* list = nullptr;
public:
	ListItemProvider();
	virtual ~ListItemProvider();

	virtual int getItemCount() { return 0; };
	/**
	 * @brief Requests from Provider element that is no index position out of givent count of visible elements
	 * @param index Index of the visible element.
	 * @param firstElement Index of the first element on the displayed list.
	 * @param prevElement Index of the previous element.
	 * @param count Number of the elements visible in the list view widget.
	 * @param topDown defines if elements are to be filled starting from the top of list
	 */
	virtual ListItem* getItem( int index, int firstElement, int prevElement, uint32_t count, bool topDown = true );
};

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_LISTITEMPROVIDER_HPP_ */
