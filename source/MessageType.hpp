// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#ifndef SOURCE_MESSAGETYPE_HPP_
#define SOURCE_MESSAGETYPE_HPP_

enum class MessageType
{

    MessageTypeUninitialized = 0,
    // eink messages
    EinkMessage,
    // gui messages
    GUIMessage,
    GUIFocusInfo, ///< information about application that gained focus
    // DB messages
    DBServiceEvents,

    DBServiceNotification, ///< Common service-db notification message.
    DBServiceBackup,

    DBSettingsGet,    ///< get current settings from database
    DBSettingsUpdate, ///< update settings

    DBSMSAdd,                         ///< Add new sms record
    DBSMSRemove,                      ///< Remove specified SMS record
    DBSMSUpdate,                      ///< Update specified SMS record
    DBSMSGetSMSLimitOffset,           ///< Get SMS records by limit,offset
    DBSMSGetSMSLimitOffsetByThreadID, ///< Get SMS records by limit,offset with specified ThreadID field
    DBSMSGetLastRecord,               ///< Get last edited record
    DBSMSGetCount,                    ///< Get all sms count

    DBThreadGet,            ///< Get thread by ID
    DBThreadGetForContact,  ///< Get thread between 2 contacts
    DBThreadRemove,         ///< Remove thread by ID
    DBThreadGetLimitOffset, ///< Get Thread record by limit,offset
    DBThreadGetCount,       ///< get Thread count
    DBThreadUpdate,         ///< Thread update

    DBSMSTemplateAdd,            ///< Add new sms template record
    DBSMSTemplateRemove,         ///< Remove selected sms template record
    DBSMSTemplateUpdate,         ///< Update selected sms template record
    DBSMSTemplateGetLimitOffset, ///< Get sms templates records by limit,offset
    DBSMSTemplateGetCount,       ///< Get sms templates reocrds count

    DBContactVerify, ///< checks database for a contact that has the same name (primary+" "+alternative) or phone number
                     ///< 1 or phone number 2 or speed dial key
    DBContactGetByName, ///< used to ask for a contact with specified primary and alternative name.
    DBContactSearch,
    DBContactGetByID,        ///< used to ask for a contact with specified id
    DBContactGetBySpeedDial, ///< used to ask for a contact with specified speed dial key assigned
    DBContactGetByNumber,    ///< used to ask for a contact with specified primary or secondary phone number
    DBContactMatchByNumber,  ///< used to best match with a single contact using a phone number (primary or secondary)
    DBContactAdd,            ///< Add contact record
    DBContactRemove,         ///< Remove contact remove
    DBContactUpdate,         ///< Update contact remove
    DBContactGetLimitOffset, ///< Get contact records by limit,offset
    DBContactGetCount,       ///< Get contacts count
    DBContactBlock,
    DBAlarmAdd,            ///< Add alarm record
    DBAlarmRemove,         ///< Remove alarm remove
    DBAlarmUpdate,         ///< Update alarm remove
    DBAlarmGetLimitOffset, ///< Get alarm records by limit,offset
    DBAlarmGetCount,       ///< Get alarm count
    DBAlarmGetNext,        ///< Get alarm, closest or equal to current timestamp.

    DBCountryCode,
    DBQuery,

    // Cellular messages
    CellularStateRequest,       ///< cellular change state request, only for use by cellular
    CellularNotification,       ///< Async notification message
    CellularAnswerIncomingCall, ///< Answer incoming call
    CellularHangupCall,         ///< Hang up call
    CellularCall,               ///< Call related events
    CellularCallRequest,        ///< Call request

    CellularListCurrentCalls,
    CellularSimProcedure,        // Broadcast on sim state changed
    CellularSimResponse,         // Send to PIN window (show, error state, hide)
    CellularSimVerifyPinRequest, // Send from PIN window with PIN, PUK, ... number

    CellularGetOwnNumber,
    CellularGetIMSI,
    CellularGetNetworkInfo,
    CellularStartOperatorsScan,
    CellularOperatorsScanResult,
    CellularNetworkInfoResult,
    CellularSelectAntenna,
    CellularSetScanMode,
    CellularGetScanMode,
    CellularGetScanModeResult,
    CellularGetFirmwareVersion,       ///< Asks for current firmware version
    CellularGetFirmwareVersionResult, ///< Returns current firmware version
    CellularGetChannel,               ///< Asks for channel, requres chnnel name
    CellularGetChannelResponse,       ///< Returns channel (and it's name)
    CellularGetCSQ,
    CellularGetCREG,
    CellularGetNWINFO,
    CellularGetAntenna,
    CellularTransmitDtmfTones,
    CellularUSSDRequest,
    CellularTimeUpdated,
    CellularSimState,
    CellularMMIData,

    DBNotesAdd,            ///< Add new note's record
    DBNotesRemove,         ///< Remove selected note's record
    DBNotesUpdate,         ///< Update selected note's record
    DBNotesGetLimitOffset, ///< Get notes records by limit,offset
    DBNotesGetCount,       ///< Get notes reocrds count

    DBCalllogAdd,            ///< Add new note's record
    DBCalllogRemove,         ///< Remove selected note's record
    DBCalllogUpdate,         ///< Update selected note's record
    DBCalllogGetLimitOffset, ///< Get Calllog records by limit,offset
    DBCalllogGetCount,       ///< Get Calllog reocrds count

    // Audio service messages
    AudioMessage,

    // application manager
    APMAction,          ///< Used to send an action request to application manager.
    APMCheckAppRunning, ///< check if application is running in application manager
    APMSwitch,          ///< request to switch to given application, optionally also to specified window
    APMSwitchPrevApp,   ///< Request to switch to previous application.
    APMConfirmSwitch,   ///< Used when application confirms that it is loosing focus and also when application confirms
                        ///< that is has gained focus
    APMConfirmClose,    ///< Sent by application to confirm completion of the close procedure
    APMConfirmWindowSwitch, ///<
    APMRegister,       ///< when application finishes initHandler it is sending this messag to inform whether init was
                       ///< successful or not.
    APMInit,           ///< Send by application to confirm completion its initialisation process
    APMDelayedClose,   ///< this message is sent internally from and to application manager to close specified
                       ///< application.
    APMChangeLanguage, ///< this message is sent from any application to inform application manager that it should send
                       ///< gui rebuild command to all applications in background and currently active application.
    APMClose, ///< this message will trigger application manager to close itself, all running applications gui and eink
              ///< services.
    APMPreventBlocking,   ///< Prevents application manager from initializing device blocking.
    APMInitPowerSaveMode, ///< This message is sent to application manager from desktop when locking timeout has
                          ///< triggered or user explicitly locked the phone.

    // keyboard messages
    KBDKeyEvent,

    AppMessage, ///< generic application message
    AppAction,  ///< application action request.
    AppSwitch,  ///< application receives this message from application manager. It a signal to gain or loose focus.
    AppSwitchWindow, ///< This is internal message transmitted within application to change window. Additional command
                     ///< and data are transmitted with it.
    AppInputEvent,   ///< used after key event translation to send input event to application
    AppRefresh,
    AppRebuild, ///< message used to notify application that it should recreate it's GUI (i.e. language has beed changed
                ///< by the user)
    AppClose,
    AppFocus,
    AppFocusLost,

    EVMFocusApplication,
    EVMKeyboardProfile,

    // Power manager
    PMChangePowerMode,

    // battery charger messages
    EVMBatteryLevel,
    EVMChargerPlugged,
    // rtc messages
    EVMMinuteUpdated, ///< This message is send to current focused application on every minute time change.
    EVMTimeUpdated,   ///< This message is send on every time update.
    // Torch messages
    EVMTorchStateMessage,
    // Keypad backlight control messages
    EVMKeypadBacklightMessage,
    // Screen frontlight control messages
    EVMScreenLightControlMessage,

    // cellular messages
    EVMGetBoard,
    EVMModemStatus,
    EVMRingIndicator,

    // bluetooth messages
    BluetoothRequest,
    BluetoothScanResult,
    BluetoothAddrResult,
    BluetoothPairResult,
    BluetoothAudioRegister,
    BluetoothDeviceMetadata,
    BluetoothRequestStream,

    LwIP_request,
    EVM_GPIO,
    SIMTrayEvent,

    // service-desktop  messages
    UpdateOS,
    Backup,
    Restore,
    Factory,
    DeveloperModeRequest,

    // FOTA messages
    HttpRequest,
    HttpResponse,
    FotaConfigureAPN,
    FotaInternetConnect,
    FotaInternetDisconnect,
    FotaInternetNotification,
    FotaStart,
    FotaProgress,
    FotaFinished,

    // Time messages
    TimersProcessingStart,
    TimersProcessingStop,
    ReloadTimers,

    StateChange,
    // antenna messages
    AntennaChanged,
    AntennaCSQChange,
    AntennaLockService,
    AntennaGetLockState,
    AntennaLockNotification,
    Settings,
    FileContentModified,
    FileIndexer
};

#endif /* SOURCE_MESSAGETYPE_HPP_ */
