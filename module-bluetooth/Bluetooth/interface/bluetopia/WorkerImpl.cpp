#include "WorkerImpl.hpp"

#include <segger/log/log.hpp>

#include <cstdio>

extern "C"
{
#include <HCITRANS.h>
#include <BTPSKRNL.h>
#include <BSCAPI.h>
#include <HCIAPI.h>
#include <GAPAPI.h>
#include <L2CAPAPI.h>
};

using namespace bsp;

// WORK IN PROGRESS - in open() in A3DP example right now (Stack initialization...)
struct HCI
{
    HCI_DriverInformation_t driver;
    HCI_HCILLConfiguration_t ll;
    HCI_Driver_Reconfigure_Data_t driver_reconfigure;
    HCI()
    {
        memset(&driver, 0, sizeof driver);
        memset(&ll, 0, sizeof ll);
        memset(&driver_reconfigure, 0, sizeof driver_reconfigure);
    }
    ~HCI()
    {}
};

BluetoothWorker *WorkerImpl::create()
{
    return new WorkerImpl();
}

WorkerImpl::WorkerImpl() : active_features(0)
{
    LOG_INFO("Create bluetooth worker");
    bt   = Bluetopia::getInstance();
    host = std::make_unique<BtDev>(BtDev());
    hci  = std::make_unique<HCI>(HCI());
    bt->open();
    if (initialize_stack() != SuccessBt) {
        LOG_ERROR("initialize_stack error!");
        return;
    }
    if (enable_a2dp() != SuccessBt) {
        LOG_ERROR("enable a2dp error!");
        return;
    }
    if (get_local_addr() != SuccessBt) {
        LOG_ERROR("get local address error!");
    }
    if (enable_l2ca() != SuccessBt) {
        LOG_ERROR("enable l2ca error");
    }
    if (hci_write_link_policy() != SuccessBt) {
        LOG_ERROR("HCI write link policy error");
    }
    if (check_a2dp() != SuccessBt) {
        LOG_ERROR("Check a2dp error");
    }
    if (aud_init() != SuccessBt) {
        LOG_ERROR("AUD init error");
    }
    if (hcill_setup() != SuccessBt) {
        LOG_ERROR("hcill_setup error");
    }
}

WorkerImpl::~WorkerImpl()
{}

int lib_log_callback(int len, char *msg)
{
    LOG_INFO("%.*s", len, msg);
    return 0;
}

BluetoothWorker::Error WorkerImpl::initialize_stack()
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    Error ret  = SuccessBt;
    int result = 0;
    BTPS_Initialization_t BTPS_Initialization;

    HCI_DRIVER_SET_COMM_INFORMATION(&hci->driver, 1, bt->default_baudrate, cpHCILL_RTS_CTS);
    hci->driver.DriverInformation.COMMDriverInformation.InitializationDelay = 100;
    BTPS_Initialization.MessageOutputCallback                               = lib_log_callback;
    // initialize kernel
    BTPS_Init(&BTPS_Initialization);

    do {
        if ((result = BSC_Initialize(&hci->driver, 0)) <= 0) {
            LOG_ERROR("HCI Initialization failure");
            ret = ErrorBtAPI;
            break;
        }
        else {
            stack.id = result;
        }

        HCI_Version_t v_hci;
        if (HCI_Version_Supported(stack.id, &v_hci) != 0) {
            LOG_ERROR("HCI supported stack err");
            ret = ErrorBtAPI;
        }
        else {
            // print supported stack
        }
    } while (0);

    // HCI_Read_Local_Version_Information(BluetoothStackID, &FW_Version_Details.StatusResult,
    // &FW_Version_Details.HCI_VersionResult, &FW_Version_Details.HCI_RevisionResult,
    // &FW_Version_Details.LMP_VersionResult, &FW_Version_Details.Manufacturer_NameResult,
    // &FW_Version_Details.LMP_SubversionResult);
    return ret;
}

BluetoothWorker::Error WorkerImpl::enable_a2dp()
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    Error ret = SuccessBt;
    if (BSC_EnableFeature(stack.id, BSC_FEATURE_A3DP_SOURCE) != 0) {
        LOG_ERROR("A2DP enable feature failure");
        ret = ErrorBtAPI;
    }
    return ret;
}

BluetoothWorker::Error WorkerImpl::get_local_addr()
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    Error ret = SuccessBt;
    if (!GAP_Query_Local_BD_ADDR(stack.id, host->val)) {
        LOG_INFO("Local address: %s", host->tostr().c_str());
    }
    else {
        ret = ErrorBtAPI;
        LOG_ERROR("Local address querry failure");
    }
    return ret;
}

BluetoothWorker::Error WorkerImpl::enable_l2ca()
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    Error ret = SuccessBt;
    LOG_INFO("Enable l2ca");
    L2CA_Link_Connect_Params_t L2CA_Link_Connect_Params;
    L2CA_Link_Connect_Params.L2CA_Link_Connect_Request_Config  = cqAllowRoleSwitch;
    L2CA_Link_Connect_Params.L2CA_Link_Connect_Response_Config = csMaintainCurrentRole;
    L2CA_Set_Link_Connection_Configuration(stack.id, &L2CA_Link_Connect_Params);
    return ret;
}

BluetoothWorker::Error WorkerImpl::hci_write_link_policy()
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    Error ret = SuccessBt;
    unsigned char status;
    LOG_INFO("HCI write link policy");
    if (HCI_Command_Supported(stack.id, HCI_SUPPORTED_COMMAND_WRITE_DEFAULT_LINK_POLICY_BIT_NUMBER) > 0)
        HCI_Write_Default_Link_Policy_Settings(
            stack.id,
            (HCI_LINK_POLICY_SETTINGS_ENABLE_MASTER_SLAVE_SWITCH | HCI_LINK_POLICY_SETTINGS_ENABLE_SNIFF_MODE),
            &status);
    return ret;
}

BluetoothWorker::Error WorkerImpl::check_a2dp()
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    Error err = SuccessBt;
    LOG_INFO("Check a2dp source enabled");
    if (!((BSC_QueryActiveFeatures(stack.id, &active_features) == 0) && (active_features & BSC_FEATURE_A3DP_SOURCE))) {
        err = ErrorBtAPI;
    }
    return err;
}

// SetConnect -> SetPair -> SetDisconnectable

void sleep_cb(Boolean_t SleepAllowed, unsigned long CallbackParameter)
{
    LOG_ERROR("not implemented");
}

BluetoothWorker::Error WorkerImpl::hcill_setup()
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    Error ret = SuccessBt;
    int result;
    if ((hci->driver.DriverInformation.COMMDriverInformation.Protocol == cpHCILL) ||
        (hci->driver.DriverInformation.COMMDriverInformation.Protocol == cpHCILL_RTS_CTS)) {
        hci->ll.SleepCallbackFunction              = sleep_cb;
        hci->ll.SleepCallbackParameter             = 0;
        hci->driver_reconfigure.ReconfigureCommand = HCI_COMM_DRIVER_RECONFIGURE_DATA_COMMAND_CHANGE_HCILL_PARAMETERS;
        hci->driver_reconfigure.ReconfigureData    = (void *)&hci->ll;

        // Register the sleep mode callback.  Note that if this unction returns greater than 0 then sleep is currently
        // enabled.
        result = HCI_Reconfigure_Driver((unsigned int)result, FALSE, &hci->driver_reconfigure);
        if (result > 0) {
            LOG_INFO("Sleep is allowed.");
        }
    }
    return ret;
}
