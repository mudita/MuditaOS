/*
 * Margins.h
 *
 *  Created on: 7 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_MARGINS_HPP_
#define MIDDLEWARES_GUI_WIDGETS_MARGINS_HPP_

namespace gui {

class Margins {
public:
	short left, top, right, bottom;

	Margins();
	Margins( const short left, const short top, const short right, const short bottom );
	virtual ~Margins();
};

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_MARGINS_HPP_ */
