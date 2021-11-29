// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

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

    DBContactGetByID [[deprecated]],        ///< used to ask for a contact with specified id
    DBContactGetBySpeedDial [[deprecated]], ///< used to ask for a contact with specified speed dial key assigned
    DBContactMatchByNumberID [[deprecated]],
    DBContactMatchByNumber
    [[deprecated]], ///< used to best match with a single contact using a phone number (primary or secondary)
    DBContactAdd [[deprecated]],    ///< Add contact record
    DBContactRemove [[deprecated]], ///< Remove contact remove
    DBContactUpdate [[deprecated]], ///< Update contact remove

    DBQuery,

    DBCalllogAdd [[deprecated]],    ///< Add new note's record
    DBCalllogRemove [[deprecated]], ///< Remove selected note's record
    DBCalllogUpdate [[deprecated]], ///< Update selected note's record

    // Audio service messages
    AudioMessage,

    // application manager
    APMGeneric,
    APMAction,          ///< Used to send an action request to application manager.
    APMFinish,          ///< Used to finish the last occurrence of the requesting application.
    APMCheckAppRunning, ///< check if application is running in application manager
    APMSwitch,          ///< request to switch to given application, optionally also to specified window
    APMSwitchPrevApp,   ///< Request to switch to previous application.
    APMConfirmSwitch,   ///< Used when application confirms that it is loosing focus and also when application confirms
                        ///< that is has gained focus
    APMConfirmClose,    ///< Sent by application to confirm completion of the close procedure
    APMConfirmWindowSwitch, ///<
    APMFinalizingClose,
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
    AppSwitchBack,

    EVMFocusApplication,
    EVMKeyboardProfile,

    // Power manager
    PMChangePowerMode,
    // System manager
    DeviceRegistration,

    // System manager
    SystemManagerCpuFrequency,
    SystemManagerSentinelRegistration,
    SystemManagerSentinelRemoval,

    // rtc messages
    EVMMinuteUpdated, ///< This message is send to current focused application on every minute time change.
    EVMTimeUpdated,   ///< This message is send on every time update.

    // cellular messages
    EVMModemStatus,
    EVMRingIndicator,

    // bluetooth messages
    BluetoothRequest,
    BluetoothScanResult,
    BluetoothAddrResult,
    BluetoothPairResult,
    BluetoothAudioStart,

    LwIP_request,
    EVM_GPIO,
    SIMTrayEvent,

    // service-desktop  messages
    UpdateOS,
    Backup,
    Restore,
    Factory,
    DeveloperModeRequest,
    DeveloperModeMessageWrapper,
    PasscodeRequest,
    TransferTimer,

    USBConnected,
    USBConfigured,
    USBDisconnected,

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

    StateChange,
    // antenna messages
    AntennaChanged,
    AntennaCSQChange,
    AntennaLockService,
    AntennaGetLockState,
    AntennaLockNotification,
    Settings,
    FileContentModified,
    FileIndexer,

    // Screen frontlight control messages
    ScreenLightControlAction,
    ScreenLightControlParameters,
    ScreenLightControlParametersResponse,

    // Vibrator messages
    VibratorPulseMessage,
    VibratorLevelMessage,

    // Quotes messages
    Quotes,

    // Alarm messages
    AlarmMessage,

    // Bell Latch state message
    LatchStateMessage
};
