#include "BluetoothWorker.hpp"
#include "profiles/GAVD.hpp"
#include "log/log.hpp"

#include <WorkerImpl.hpp>

using namespace bsp;

BluetoothWorker *BluetoothWorker::create()
{
    return WorkerImpl::create();
}

BluetoothWorker::BluetoothWorker()
{
}

BluetoothWorker::~BluetoothWorker()
{
}

BluetoothWorker::Error BluetoothWorker::aud_init()
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    Error err = SuccessBt;
    LOG_INFO("Initialize AUD profile");
    BtProfile *prof = new GAVD();
    profiles.push_back(prof);
    if (prof->init(&stack) != BtProfile::SuccessBtProfile) {
        LOG_ERROR("AUD init failure!");
    } else {
        // ASSIGN_CLASS_OF_DEVICE(ClassOfDevice, 0x28, 0x04, 0x10);
    }
    return err;
}
