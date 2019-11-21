/*
 * ThreadItem.hpp
 *
 *  Created on: 15 lis 2019
 *      Author: kuba
 */

#pragma once

#include "Label.hpp"
#include "Image.hpp"
#include "ListItem.hpp"
#include "ThreadModel.hpp"

namespace gui{

class ThreadItem : public ListItem{

private:
	ThreadModel* model = nullptr;

	std::shared_ptr<ThreadRecord> thread = nullptr;
	gui::Label* contact = nullptr;
	gui::Label* timestamp = nullptr;
	gui::Label* preview = nullptr;
public:
	ThreadItem( ThreadModel* model);

	bool onDimensionChanged( const BoundingBox& oldDim, const BoundingBox& newDim) override;
	void setThreadItem(std::shared_ptr<ThreadRecord>& thread);
};
} /*namespace gui*/
