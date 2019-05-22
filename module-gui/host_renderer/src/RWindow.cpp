/*
 * RWindow.cpp
 *
 *  Created on: 24 kwi 2019
 *      Author: robert
 */
#include <iostream>
#include <gtk/gtk.h>

#include "RWindow.hpp"

static gboolean
viewUpdate(gpointer data)
{
    RWindow* win = reinterpret_cast<RWindow*>(data);
    win->updateDrawBuffer();

    return true;
}

void RWindow::updateDrawBuffer() {
	for (size_t i = 0; i < width*height; i++)
		for (size_t j = 0; j < 3; j++){
			uint8_t c = shmMemPtr[i] << 4;
	          rgbMemPtr[i * 3 + j] = c;
		}
	queue_draw();
}

bool RWindow::onKeyPress(GdkEventKey* event)
{
    std::cout << event->keyval << ' ' << event->hardware_keycode << ' ' << event->state << std::endl;

    return false;
}

RWindow::RWindow( char* shmMemory, int w, int h ) :
	shmMemPtr{shmMemory},
	rgbMemPtr{nullptr},
	width{w},
	height{h} {

	rgbMemPtr = new char[3*w*h];
	drawArea.setDrawData(rgbMemPtr);

	set_title("GUI RENDERER");
	set_default_size( 480,600);

	add(drawArea);
	show_all_children();
	g_timeout_add( 1000, viewUpdate, this );

	this->signal_key_press_event().connect(
	    sigc::mem_fun(*this, &RWindow::onKeyPress), false);

}

RWindow::~RWindow() {
	if( rgbMemPtr != nullptr ) {
		delete [] rgbMemPtr;
	}
}

