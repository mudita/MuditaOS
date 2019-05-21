/*
 * Progress.hpp
 *
 *  Created on: 22 mar 2019
 *      Author: robert
 */

#ifndef MIDDLEWARES_GUI_WIDGETS_PROGRESS_HPP_
#define MIDDLEWARES_GUI_WIDGETS_PROGRESS_HPP_

#include "Rect.hpp"

namespace gui {

class Progress: public Rect {
protected:
	//Value that corresponds to 100% of the progress.
	int total;
	//current value of the progress.
	int current;
	//rectangle used to draw thin line.
	gui::Rect* thinRect;
	//rectangle used to draw thick line.
	gui::Rect* thickRect;

	/**
	 * Creates local widgets
	 */
	virtual void createWidgets();
	/**
	 * @brief Sets new sizes of thick and thin rects.
	 */
	virtual void updateProgress();
public:
	Progress();
	Progress( Item* parent, const uint32_t& x, const uint32_t& y, const uint32_t& w, const uint32_t& h);
	virtual ~Progress() {};

	/**
	 * @brief Sets user defined value that corresponds to the progress set to 100%.
	 */
	virtual void setTotalProgress( int value );
	/*
	 * @brief Sets current progress in correspondence to the value of the total progress which is corresponding to the value of 100%.
	 * I.e. value equal to one fourth of the total progress value will occupy 25% of the space assigned to the widget.
	 */
	virtual void setCurrentProgress( int value );
	/**
	 * Allows to set progress using values from 0 to 100. Other values are ignored.
	 */
	virtual void setCurrentPercent( int value );
	/*
	 * Returns value of the total progress.
	 */
	virtual int getTotalProgress() { return total; };
	/**
	 * @brief Returns value of the current progress.
	 */
	virtual int getCurrentProgress() { return current; };

	//virtaul methods from Item
	std::list<DrawCommand*> buildDrawList() override;
	bool onDimensionChanged( const BoundingBox& oldDim, const BoundingBox& newDim) override;
};

} /* namespace gui */

#endif /* MIDDLEWARES_GUI_WIDGETS_PROGRESS_HPP_ */
