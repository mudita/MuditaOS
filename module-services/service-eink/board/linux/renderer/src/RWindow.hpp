/*
 * RWindow.hpp
 *
 *  Created on: 24 kwi 2019
 *      Author: robert
 */

#ifndef RENDERER_RWINDOW_HPP_
#define RENDERER_RWINDOW_HPP_

#include <gtkmm.h>
#include <gtkmm/drawingarea.h>

#include "RArea.hpp"

class RWindow : public Gtk::Window{
protected:
	RArea drawArea;
	//pointer to shared memory with gui renderer buffer passed from main.
	char* shmMemPtr;
	//local buffer
	char* rgbMemPtr;
	//dimensions of the shared memory buffer
	int width, height;

public:
	RWindow( char* shmMemory, int w, int h );
	virtual ~RWindow();
	bool onKeyPress(GdkEventKey* event);

	void updateDrawBuffer();
};

#endif /* RENDERER_RWINDOW_HPP_ */
