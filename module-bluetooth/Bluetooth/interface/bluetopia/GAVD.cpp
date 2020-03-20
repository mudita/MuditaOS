#include "../../profiles/GAVD.hpp"
#include "BtDev.hpp"
#include "segger/log/log.hpp"

extern "C"
{
#include <AUDAPI.h>
#include <BTPSKRNL.h>
};

// TODO separate classes by used stack... --> Yuck
static BTPSCONST AUD_Stream_Format_t AudioSRCSupportedFormats[] = {
    {44100, 2, 0}, {48000, 2, 0}, {48000, 1, 0}, {44100, 1, 0}};
#define NUM_SRC_SUPPORTED_FORMATS (sizeof(AudioSRCSupportedFormats) / sizeof(AUD_Stream_Format_t))

static void BTPSAPI AUD_Event_Callback(unsigned int id,
                                       AUD_Event_Data_t *AUD_Event_Data,
                                       unsigned long CallbackParameter);
// <---
//

class GAVDi : public GAVD
{
  public:
    GAVDi();
    ErrorBtProfile init(Stack *stack) override;
};

GAVD *GAVD::create()
{
    return new GAVDi();
}

GAVDi::GAVDi()
{
    // TODO add depends
    st = State::ProfileInactive;
}

BtProfile::ErrorBtProfile GAVDi::init(Stack *stack)
{
    ErrorBtProfile err = ErrorBtProfile::SuccessBtProfile;
    LOG_INFO("GAVD init");
    if (st != ProfileActive) {
        AUD_Initialization_Info_t InitializationInfo;
        AUD_Stream_Initialization_Info_t InitializationInfoSRC;
        AUD_Remote_Control_Initialization_Info_t InitializationInfoAVR;
        AUD_Remote_Control_Role_Info_t RemoteControlRoleInfo;

        InitializationInfoSRC.InitializationFlags          = 0;
        InitializationInfoSRC.NumberConcurrentStreams      = 1;
        InitializationInfoSRC.EndpointSDPDescription       = (char *)"A3DP Source";
        InitializationInfoSRC.NumberSupportedStreamFormats = NUM_SRC_SUPPORTED_FORMATS;

        /* Initialize the stream configuration supported format list.  */
        memcpy(InitializationInfoSRC.StreamFormat, AudioSRCSupportedFormats, sizeof(AudioSRCSupportedFormats));

        RemoteControlRoleInfo.SupportedFeaturesFlags = SDP_AVRCP_SUPPORTED_FEATURES_CONTROLLER_CATEGORY_1;
        RemoteControlRoleInfo.ProviderName           = (char *)"Texas Instruments";
        RemoteControlRoleInfo.ServiceName            = (char *)"STM32 A3DP Source";

        InitializationInfoAVR.InitializationFlags = 0;
        InitializationInfoAVR.TargetRoleInfo      = &RemoteControlRoleInfo;
        InitializationInfoAVR.ControllerRoleInfo  = NULL;
        InitializationInfoAVR.SupportedVersion    = apvVersion1_0;

        InitializationInfo.InitializationFlags             = 0;
        InitializationInfo.RemoteControlInitializationInfo = &InitializationInfoAVR;
        InitializationInfo.SRCInitializationInfo           = &InitializationInfoSRC;
        InitializationInfo.SNKInitializationInfo           = NULL;
        InitializationInfo.InitializationFlags             = AUD_INITIALIZATION_INFO_FLAGS_OVERRIDE_MEDIA_MTU;
        InitializationInfo.MediaMTU                        = 800;
        if (AUD_Initialize(stack->id, &InitializationInfo, AUD_Event_Callback, 0) == 0)
            st = State::ProfileActive;
        else
            st = State::ProfileInactive;
    }
    return err;
}

static void BTPSAPI AUD_Event_Callback(unsigned int id,
                                       AUD_Event_Data_t *AUD_Event_Data,
                                       unsigned long CallbackParameter)
{
    BtDev tmp;
    if ((id) && (AUD_Event_Data)) {
        LOG_INFO("");
        switch (AUD_Event_Data->Event_Data_Type) {
        case etAUD_Open_Request_Indication:
            tmp = AUD_Event_Data->Event_Data.AUD_Open_Request_Indication_Data->BD_ADDR;
            LOG_INFO("etAUD_Open_Request_Indication");
            LOG_INFO("BD_ADDR:               %s", tmp.tostr().c_str());
            LOG_INFO("ConnectionRequestType: %d",
                     AUD_Event_Data->Event_Data.AUD_Open_Request_Indication_Data->ConnectionRequestType);
            break;
        case etAUD_Stream_Open_Indication:
            /* Occurs whenever the master device connects out to us.    */
            break;
        case etAUD_Stream_Open_Confirmation:
            tmp = AUD_Event_Data->Event_Data.AUD_Stream_Open_Confirmation_Data->BD_ADDR;
            LOG_INFO("etAUD_Stream_Open_Confirmation");
            LOG_INFO("Status:      %u", AUD_Event_Data->Event_Data.AUD_Stream_Open_Confirmation_Data->OpenStatus);
            LOG_INFO("BD_ADDR:     %s", tmp.tostr().c_str());

            //                if(AUD_Event_Data->Event_Data.AUD_Stream_Open_Confirmation_Data->OpenStatus ==
            //                AUD_STREAM_OPEN_CONFIRMATION_STATUS_SUCCESS)
            //                {
            //                    LOG_INFO("MediaMTU:    %d",
            //                    AUD_Event_Data->Event_Data.AUD_Stream_Open_Confirmation_Data->MediaMTU);
            //
            //                    OpenA3DPStream(AUD_Event_Data->Event_Data.AUD_Stream_Open_Confirmation_Data);
            //
            //                    /* Track this connection's BD_ADDR.                      */
            //                    RemoteSinkBD_ADDR =
            //                    AUD_Event_Data->Event_Data.AUD_Stream_Open_Confirmation_Data->BD_ADDR;
            //
            //                    if(PerformingAutoConnect)
            //                    {
            //                        PerformingAutoConnect = FALSE;
            //
            //                        /* If we autoconnected, then we should start playing. */
            //                        Play(NULL);
            //                    }
            //                }
            //                else
            //                {
            //                    LOG_INFO("Connection to %s failed.", tmp.tostr().c_str());
            //                    if(PerformingAutoConnect)
            //                    {
            //                        if(AutoConnectIndex < NumberofValidResponses)
            //                        {
            //                            /* Keep trying the auto-connection process.        */
            //                            AttemptNextAutoConnect();
            //                        }
            //                        else
            //                        {
            //                            LOG_INFO("Ending auto-connect process.");
            //                            PerformingAutoConnect = FALSE;
            //                        }
            //                    }
            //                }
            break;
        case etAUD_Stream_Close_Indication:
            /* Occurs when the master either closes or disconnects.     */
            tmp = AUD_Event_Data->Event_Data.AUD_Stream_Close_Indication_Data->BD_ADDR;
            LOG_INFO("etAUD_Stream_Close_Indication");
            LOG_INFO("BD_ADDR:          %s", tmp.tostr().c_str());
            LOG_INFO("StreamType:       %d", AUD_Event_Data->Event_Data.AUD_Stream_Close_Indication_Data->StreamType);
            LOG_INFO("DisconnectReason: %d",
                     AUD_Event_Data->Event_Data.AUD_Stream_Close_Indication_Data->DisconnectReason);

            //                if(AUD_Event_Data->Event_Data.AUD_Stream_Close_Indication_Data->DisconnectReason ==
            //                adrRemoteDeviceDisconnect)
            //                {
            //                    /* Close the stream normally.                            */
            //                    CloseA3DPStream();
            //                }
            //                else
            //                {
            //                    /* On link loss or timeout, the A3DP API seems to return */
            //                    /* invalid connection handle error codes if we try to    */
            //                    /* close the stream. So, instead we will manually reset  */
            //                    /* the state here.                                       */
            //                    LOG_INFO("Resetting A3DP stream state.");
            //                    A3DPOpened  = FALSE;
            //                    A3DPPlaying = FALSE;
            //                }

            break;
        case etAUD_Remote_Control_Open_Indication:
            tmp = AUD_Event_Data->Event_Data.AUD_Remote_Control_Open_Indication_Data->BD_ADDR;
            LOG_INFO("etAUD_Remote_Control_Open_Indication");
            LOG_INFO("BD_ADDR:     %s", tmp.tostr().c_str());
            break;
        case etAUD_Remote_Control_Close_Indication:
            tmp = AUD_Event_Data->Event_Data.AUD_Remote_Control_Close_Indication_Data->BD_ADDR;
            LOG_INFO("etAUD_Remote_Control_Close_Indication");
            LOG_INFO("BD_ADDR:          %s", tmp.tostr().c_str());
            LOG_INFO("DisconnectReason: %d",
                     AUD_Event_Data->Event_Data.AUD_Remote_Control_Close_Indication_Data->DisconnectReason);
            break;
        case etAUD_Remote_Control_Command_Indication:
            LOG_INFO("Remote controll command");
            // ProcessIncomingRemoteControlCommand(AUD_Event_Data->Event_Data.AUD_Remote_Control_Command_Indication_Data);
            break;
        case etAUD_Stream_State_Change_Indication:
            break;
        case etAUD_Stream_State_Change_Confirmation:
            tmp = AUD_Event_Data->Event_Data.AUD_Stream_State_Change_Confirmation_Data->BD_ADDR;
            LOG_INFO("etAUD_Stream_State_Change_Confirmation");
            LOG_INFO("BD_ADDR:     %s", tmp.tostr().c_str());
            //                if(AUD_Event_Data->Event_Data.AUD_Stream_State_Change_Confirmation_Data->StreamState ==
            //                astStreamStarted)
            //                    //StartA3DPStream();
            //                else
            //                    //StopA3DPStream();
            //                break;
        case etAUD_Stream_Format_Change_Indication:
            tmp = AUD_Event_Data->Event_Data.AUD_Stream_Format_Change_Indication_Data->BD_ADDR;
            LOG_INFO("etAUD_Stream_Format_Change_Indication");
            LOG_INFO("BD_ADDR:     %s", tmp.tostr().c_str());
            //                if(A3DPOpened)
            //                    ReconfigureA3DPStream(&(AUD_Event_Data->Event_Data.AUD_Stream_Format_Change_Indication_Data->StreamFormat));
            break;
        case etAUD_Encoded_Audio_Data_Indication:
            break;
        case etAUD_Signalling_Channel_Open_Indication:
            tmp = AUD_Event_Data->Event_Data.AUD_Signalling_Channel_Open_Indication_Data->BD_ADDR;
            LOG_INFO("etAUD_Signalling_Channel_Open_Indication");
            LOG_INFO("BD_ADDR:  %s", tmp.tostr().c_str());
            break;
        case etAUD_Signalling_Channel_Close_Indication:
            tmp = AUD_Event_Data->Event_Data.AUD_Signalling_Channel_Close_Indication_Data->BD_ADDR, tmp.tostr().c_str();
            LOG_INFO("etAUD_Signalling_Channel_Close_Indication");
            LOG_INFO("BD_ADDR:  %s", tmp.tostr().c_str());
            LOG_INFO("DisconnectReason: %d",
                     AUD_Event_Data->Event_Data.AUD_Signalling_Channel_Close_Indication_Data->DisconnectReason);
            break;
        default:
            LOG_INFO("Unhandled AUD event: %d", AUD_Event_Data->Event_Data_Type);
            break;
        }
    }
}
