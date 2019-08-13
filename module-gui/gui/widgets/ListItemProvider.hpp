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
	virtual ListItem* getItem( int index );
};

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_LISTITEMPROVIDER_HPP_ */
