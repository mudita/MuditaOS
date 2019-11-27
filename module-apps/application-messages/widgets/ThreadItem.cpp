/*
 * ThreadItem.cpp
 *
 *  Created on: 15 lis 2019
 *      Author: kuba
 */
#include "ThreadItem.hpp"
#include "time/time_conversion.hpp"

#include "service-db/api/DBServiceAPI.hpp"

#include <Style.hpp>
#include "../MessagesStyle.hpp"

namespace gui {

ThreadItem::ThreadItem(ThreadModel *threadmodel) {
	using namespace style;

	model = threadmodel;
	minWidth = 431;
	minHeight = 100;
	maxWidth = 431;
	maxHeight = 100;

	setRadius(0);
	setEdges(
			RectangleEdgeFlags::GUI_RECT_EDGE_BOTTOM
					| RectangleEdgeFlags::GUI_RECT_EDGE_TOP);

	setPenFocusWidth(window::default_border_focucs_w);
	setPenWidth(window::default_border_no_focus_w);

	contact = new gui::Label(this, 0, 0, 0, 0);
	contact->setPenFocusWidth(window::default_border_no_focus_w);
	contact->setPenWidth(window::default_border_no_focus_w);
	contact->setFont(style::window::font::big);
	contact->setDotsMode(true);
	contact->setAlignement(gui::Alignment {
			gui::Alignment::ALIGN_HORIZONTAL_LEFT,
			gui::Alignment::ALIGN_VERTICAL_CENTER });

	timestamp = new gui::Label(this, 0, 0, 0, 0);
	timestamp->setPenFocusWidth(window::default_border_no_focus_w);
	timestamp->setPenWidth(window::default_border_no_focus_w);
	timestamp->setFont(style::window::font::small);
	timestamp->setDotsMode(true);
	timestamp->setAlignement(gui::Alignment {
			gui::Alignment::ALIGN_HORIZONTAL_RIGHT,
			gui::Alignment::ALIGN_VERTICAL_CENTER });

	preview = new gui::Label(this, 0, 0, 0, 0);
	preview->setPenFocusWidth(window::default_border_no_focus_w);
	preview->setPenWidth(window::default_border_no_focus_w);
	preview->setFont(style::window::font::small);
	preview->setDotsMode(true);
	preview->setAlignement(gui::Alignment {
			gui::Alignment::ALIGN_HORIZONTAL_LEFT,
			gui::Alignment::ALIGN_VERTICAL_CENTER });

}

bool ThreadItem::onDimensionChanged(const BoundingBox &oldDim,
		const BoundingBox &newDim) {
	using namespace messages::threadItem;

	contact->setPosition(contactPositionX, contactPositionY);
	contact->setSize(newDim.w - cotactWidthOffset, newDim.h / 2);

	timestamp->setPosition(newDim.w - timestampWidth, 0);
	timestamp->setSize(timestampWidth, newDim.h / 2);

	preview->setPosition(14, newDim.h / 2);
	preview->setSize(newDim.w - previewWidthOffset, newDim.h / 2);

	return true;
}

void ThreadItem::setThreadItem(std::shared_ptr<ThreadRecord> &thread) {
	this->thread = thread;

	if (model != nullptr) {
		auto contactRec = DBServiceAPI::ContactGetByID(model->getApplication(),
				thread->contactID);
		auto cont = contactRec->front();
		contact->setText(cont.primaryName + " " + cont.alternativeName);
	}

	timestamp->setText(utils::time::SysTime(thread->date));

	preview->setText(thread->snippet);

}

} /*namespace gui*/

