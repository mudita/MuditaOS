#include "BluetoothWorker.hpp"
#include "log/log.hpp"
#include "BtCommand.hpp"

using namespace bsp;

BluetoothWorker::BluetoothWorker(sys::Service* service) : Worker(service) {
    init({{"qBtIO", sizeof(Bt::Message), 10},
          {"qBtWork", sizeof(Bt::EvtWorker), 10},
         });
};

BluetoothWorker::~BluetoothWorker()
{
    if(this->bt_worker_task!= nullptr) {
        // TODO kill task
    }
    LOG_INFO("Worker removed");
}

bool BluetoothWorker::run() {
    if(is_running) {
        return true;
    }
    if(Worker::run()) {
        // TODO what - that's just wrong -_-
        auto el = getQueues()[queueIO_handle];
        BlueKitchen::getInstance()->qHandle = el;
        Bt::initialize_stack();
        Bt::GAP::register_scan();
        std::string name = "PurePhone";
        Bt::set_name(name);
        // set local namne
        // set discoverable (on)
        // Bt::GAP::
        Bt::run_stack(&this->bt_worker_task);
        is_running = true;
        return true;
    } else {
        return false;
    }
}

bool BluetoothWorker::scan()
{
    std::vector<Device> empty;
    /// TODO -> create worker for GAP - this is async... handle from WorkerImpl? (message for each/all elements found)
    // if state inactive -> return not ready or queue?
    // state active - change what? (BT [ON] on display? )
    auto ret = Bt::GAP::scan();
    if(ret.err != Bt::Error::Succes) {
        LOG_ERROR("Cant start scan!: %d %d", ret.err, ret.lib_code);
        return false;
    } else {
        LOG_INFO("Scan started!");
        return true;
    }
}

bool BluetoothWorker::set_visible()
{
    LOG_ERROR("TODO");
    return false;
}

bool BluetoothWorker::start_pan()
{
    auto err = Bt::PAN::bnep_setup();
    if(err.err != Bt::Error::Succes) {
        LOG_ERROR("PAN setup error: %d %d", err.err, err.lib_code);
    }
    return false;
}

BluetoothWorker::Error BluetoothWorker::aud_init()
{
    LOG_INFO("%s", __PRETTY_FUNCTION__);
    Error err = SuccessBt;
    LOG_INFO("AUDIO - TODO");
    // start GAVD
    // && ASSIGN_CLASS_OF_DEVICE(ClassOfDevice, 0x28, 0x04, 0x10);
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
