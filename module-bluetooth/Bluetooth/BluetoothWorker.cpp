#include "BluetoothWorker.hpp"
#include "profiles/GAVD.hpp"
#include "profiles/GAP.hpp"
#include "log/log.hpp"

#include <WorkerImpl.hpp>

using namespace bsp;

BluetoothWorker::BluetoothWorker(sys::Service* service) : Worker(service) {
    init({{"qBtIO", sizeof(Bt::Message), 10},
          {"qBtWork", sizeof(Bt::EvtWorker), 10},
         });
    /// well this crashes on malloc? O_O
    if(run()) {
        // TODO what - that's just wrong -_-
        auto el = getQueues()[queueIO_handle];
        BlueKitchen::getInstance()->qHandle = el;
        initialize_stack();
        pan_bnep_setup();
        run_stack(&this->bt_worker_task);
    }
};

BluetoothWorker::~BluetoothWorker()
{
    if(this->bt_worker_task!= nullptr) {
        // TODO kill task
    }
    LOG_INFO("Worker removed");
}

std::vector<Device> BluetoothWorker::scan()
{
    /// TODO -> create worker for GAP
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
#ifdef DO_DEBUG_HCI_COMS
            LOG_INFO("[evt] sent");
#endif
            if(bt->write_done_cb) {
                bt->write_done_cb();
            }
            break;
        case Bt::Message::EvtReceived:
            {

                for( int i=0; i < bt->to_read_req; ++i) {
                    // error in pop should never happen
                    if(int ret = bt->in.pop((char*)bt->read_buff+i)) {
                        LOG_ERROR("This shall never happen: %d", ret);
                    }
                }
#ifdef DO_DEBUG_HCI_COMS
                std::stringstream ss;
                for (int i =0; i<bt->to_read_req; ++i) {
                    ss << " 0x" << std::hex << (int)*(bt->read_buff+i);
                }
                LOG_DEBUG("[evt] recieved <-- [%d]>%s<", bt->to_read_req, ss.str().c_str());
#endif
                bt->to_read_req = 0;

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
