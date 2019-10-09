#include "BluetoothWorker.hpp"
#include "profiles/GAVD.hpp"
#include "profiles/GAP.hpp"
#include "log/log.hpp"

#include <WorkerImpl.hpp>

using namespace bsp;

BluetoothWorker::BluetoothWorker(sys::Service* service) : Worker(service) {
  init({{"qBluetooth", sizeof(Bt::Message), 10}});
  run();
  // TODO what - that's just wrong -_-
  auto el = getQueues()[queueIO_handle];
  BlueKitchen::getInstance()->qHandle = el;
  initialize_stack(getQueues()[queueIO_handle]);
};

BluetoothWorker::~BluetoothWorker()
{
    LOG_INFO("Worker removed");
}

std::vector<Device> BluetoothWorker::scan()
{
    std::vector<Device> empty;
    GAP *prof = GAP::create();
    prof->scan();
    return empty;
}

BluetoothWorker::Error BluetoothWorker::aud_init()
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    Error err = SuccessBt;
    LOG_INFO("Initialize AUD profile");
    BtProfile *prof = GAVD::create();
    if (prof->init(&stack) != BtProfile::SuccessBtProfile) {
        LOG_ERROR("AUD init failure!");
    } else {
        // ASSIGN_CLASS_OF_DEVICE(ClassOfDevice, 0x28, 0x04, 0x10);
    }
    return err;
}

#include <sstream>

bool BluetoothWorker::handleMessage( uint32_t queueID ) {

    QueueHandle_t queue = queues[queueID];
    if(queueID == queueService) {
            LOG_DEBUG("not interested");
            return true;
    }
    if(queueID != queueIO_handle) {
        LOG_ERROR("Wrong queue! %d", queueID);
        return false;
    }

    Bt::Message notification;
    if( xQueueReceive(queue, &notification, 0 ) != pdTRUE ) {
        LOG_ERROR("Receive failure!");
        return false;
    }
    auto bt = BlueKitchen::getInstance();
    switch (notification) {
        case Bt::Message::EvtSent:
            LOG_INFO("[evt] sent");
            if(bt->write_done_cb) {
                bt->write_done_cb();
            }
            break;
        case Bt::Message::EvtReceived :
            {
                std::stringstream ss;
                for (int i =0; i<bt->to_read_debug; ++i) {
                    ss << " 0x" << std::hex << (int)*(bt->read_buff-i);
                }
                LOG_DEBUG("[evt] recieved <-- [%d]>%s<", bt->to_read_debug, ss.str().c_str());

                if(bt->read_ready_cb) {
                    bt->read_ready_cb();
                }
            }
            break;
        case Bt::Message::EvtSentError:
        case Bt::Message::EvtRecError:
        case Bt::Message::EvtUartError:
        case Bt::Message::EvtRecUnwanted:
            {
                LOG_ERROR("Uart error [%d]: %s", notification, Bt::MessageCstr(notification));
                break;
            }
            break;
        default:
            LOG_ERROR("ERROR");
    }

    return true;
}
