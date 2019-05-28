/*
 * RArea.hpp
 *
 *  Created on: 24 kwi 2019
 *      Author: robert
 */

#ifndef RENDERER_RAREA_HPP_
#define RENDERER_RAREA_HPP_

#include <cairomm/context.h>
#include <gtkmm.h>
#include <gtkmm/drawingarea.h>

class RArea : public Gtk::DrawingArea{
public:
	RArea();
	virtual ~RArea();

	void setDrawData( char* data ) { this->data = data; };
protected:
	char* data;
	bool on_draw (const Cairo::RefPtr<Cairo::Context>& cr) override;
};

#endif /* RENDERER_RAREA_HPP_ */
