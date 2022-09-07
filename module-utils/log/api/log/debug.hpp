// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#define DEBUG_APPLICATION_MANAGEMENT 0 /// show verbose logs in ApplicationManager
#define DEBUG_BLUETOOTH_HCI_COMS     0 /// show communication with BT module - transactions
#define DEBUG_BLUETOOTH_HCI_BYTES    0 /// show communication with BT module - all the HCI bytes
#define DEBUG_CELLULAR_UART          0 /// show full modem uart communication
#define DEBUG_DB_MODEL_DATA          0 /// show messages prior to handling in service
#define DEBUG_EINK_REFRESH           0 /// show bounding boxes and frames in partial refresh
#define DEBUG_FONT                   0 /// show Font debug messages
#define DEBUG_GUI_TEXT               0 /// show basic debug messages for gui::Text - warning this can be hard on cpu
#define DEBUG_GUI_TEXT_LINES         0 /// show extended debug messages for gui::Text - lines building
#define DEBUG_GUI_TEXT_CURSOR        0 /// show extended debug messages for gui::Text - cursor handling
#define DEBUG_INPUT_EVENTS           0 /// show input events prints in system
#define DEBUG_MISSING_ASSETS         0 /// show debug concerning missing assets
#define DEBUG_SCOPED_TIMINGS         0 /// show timings in measured functions
#define DEBUG_SETTINGS_DB            0 /// show extensive settings logs for all applications
#define DEBUG_SERVICE_CELLULAR       0 /// show various logs in cellular service
#define DEBUG_SERVICE_MESSAGES       0 /// show messages prior to handling in service
#define DEBUG_SIM_IMPORT_DATA        0 /// show messages connected to sim data imports
#define DEBUG_TIMER                  0 /// debug timers system utility
