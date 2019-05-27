/*
 * RWindow.hpp
 *
 *  Created on: 24 kwi 2019
 *      Author: robert
 */

#ifndef RENDERER_RWINDOW_HPP_
#define RENDERER_RWINDOW_HPP_

#include <map>
#include <cstdint>
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
	//map that holds pair of key, time
	//	pair is inserted into map when there is key event of GDK_KEY_PRESS
	//it is removed when there is GDK_KEY_RELEASE event
	std::map<uint32_t, uint32_t> keys;

public:
	RWindow( char* shmMemory, int w, int h );
	virtual ~RWindow();
	bool onKeyPress(GdkEventKey* event);
	bool onKeyRelease(GdkEventKey* event);

	void updateDrawBuffer();
};

#endif /* RENDERER_RWINDOW_HPP_ */
