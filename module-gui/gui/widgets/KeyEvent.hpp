///*
// * KeyEvent.hpp
// *
// *  Created on: 25 kwi 2019
// *      Author: robert
// */
//
//#ifndef GUI_WIDGETS_KEYEVENT_HPP_
//#define GUI_WIDGETS_KEYEVENT_HPP_
//
//#include <cstdint>
//
// namespace gui {
//
// enum class KeyCode {
//	KEY_LEFT,
//	KEY_UP,
//	KEY_RIGHT,
//	KEY_DOWN,
//	KEY_LF,
//	KEY_RF,
//	KEY_ENTER,
//	KEY_0,
//	KEY_1,
//	KEY_2,
//	KEY_3,
//	KEY_4,
//	KEY_5,
//	KEY_6,
//	KEY_7,
//	KEY_8,
//	KEY_9,
//	KEY_AST,
//	KEY_PND,
//	KEY_GREEN,
//	KEY_RED,
//	KEY_VOLUP,
//	KEY_VOLDN,
//	KEY_TORCH,
//	SWITCH_UP,
//	SWITCH_MID,
//	SWITCH_DN
//};
//
// enum class KeyState {
//	KEY_UNKNOWN         = 0x00,
//	KEY_PRESSED         = 0x01,
//    KEY_RELEASED_SHORT  = 0x02,
//    KEY_RELEASED_LONG   = 0x04,
//	KEY_CYCLE           = 0x08
//};
//
// class KeyEvent {
// public:
//	//defines mapped code of the pressed button
//	KeyCode keyCode;
//	//defines state of the button
//	KeyState keyState;
//	//defines how long button was pressed
//	uint32_t duration;
//
//	KeyEvent();
//	virtual ~KeyEvent();
//};
//
//} /* namespace gui */
//
//#endif /* GUI_WIDGETS_KEYEVENT_HPP_ */
