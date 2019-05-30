/*
 * ServiceGUI.hpp
 *
 *  Created on: 22 maj 2019
 *      Author: robert
 */

#ifndef MODULE_SERVICES_SERVICE_GUI_SERVICEGUI_HPP_
#define MODULE_SERVICES_SERVICE_GUI_SERVICEGUI_HPP_

//module-gui
#include "gui/core/Context.hpp"
#include "gui/core/Renderer.hpp"

#include "Service/Service.hpp"
#include "Service/Message.hpp"

//class FrameBuffer {
//	uint8_t* data;
//	uint32_t width;
//	uint32_t height;
//public:
//	FrameBuffer( uint32_t w, uint32_t h ) :width{w}, height{h} {
//		//prevent from creating buffer with 0 size
//		if( width == 0 )
//			width = 1;
//		if( height == 0 )
//			height = 1;
//		data = new uint8_t[width*height];
//	}
//	virtual ~FrameBuffer() { if( data ) delete [] data;} ;
//
//	uint8_t* getData() { return data; };
//	uint32_t getWidth() { return width; };
//	uint32_t getHeight() { return height; };
//	uint32_t getSize() { return width*height; };
//	bool copy( const FrameBuffer& buf ) {
//		if( (width == buf.width) && (height == buf.height) &&
//			(data != nullptr) && (buf.data != nullptr )) {
//			memcpy( data, buf.data, width*height );
//			return true;
//		}
//		return false;
//	}
//};

class ServiceGUI: public sys::Service {

	//this is where every incomming frame is painted.
	gui::Context* renderContext;
	//this buffer is provided to eink
	gui::Context* transferContext;
	//ID of the last rendered frame
	uint32_t renderFrameCounter;
	//ID of the last frame sent to eink for rendering
	uint32_t transferedFrameCounter;
	//Horizontal size of the screen in pixels
	uint32_t screenWidth;
	//vertical size of the screen in pixels
	uint32_t screenHeight;
	//object responsible for rendering images to context
	gui::Renderer renderer;
	//flag that defines whether eink is ready for new frame buffer
	bool einkReady = false;
	uint32_t timer_id= 0;


public:
    ServiceGUI(const std::string& name, uint32_t screenWidth, uint32_t screenHeight );
    ~ServiceGUI();

    sys::Message_t DataReceivedHandler(sys::DataMessage* msgl) override;
    // Invoked when timer ticked
    void TickHandler(uint32_t id) override;

    // Invoked during initialization
    sys::ReturnCodes InitHandler() override;

    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes WakeUpHandler() override;

    sys::ReturnCodes SleepHandler() override;

};

#endif /* MODULE_SERVICES_SERVICE_GUI_SERVICEGUI_HPP_ */
