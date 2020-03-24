#include "ParserFsm.hpp"
#include "Fsmlist.hpp"
#include "segger/log/log.hpp"

class StateMessageType;
class StateMessageSize;
class StateMessagePayload;

// ----------------------------------------------------------------------------
// State: MessageType
//

class StateMessageType : public ParserFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateMessageType ***");
        // Restore initial state of all buffers and FSMs
        msgSizeBytesToRead = parserutils::message::size_length;
        msgPayloadSizeStr.erase();
        msgPayloadSize = 0;
        msgPayload.clear();
        EndpointFsm::reset();
        RawDataFsm::reset();
    };

    void react(MessageDataEvt const &) override
    {
        LOG_DEBUG("*** react MessageTypeEvt ***");
        msgType = static_cast<parserutils::message::Type>(msgChunk.front());
        msgChunk.pop_front();

        switch (msgType) {
        case parserutils::message::Type::endpoint:
        case parserutils::message::Type::rawData:
            transit<StateMessageSize>();
            break;
        default:
            LOG_ERROR("Invalid msg type: %d", msgType);
            return;
        }
    };
};

// ----------------------------------------------------------------------------
// State: MessageSize
//

class StateMessageSize : public ParserFsm
{
    void react(MessageDataEvt const &msg) override
    {
        LOG_DEBUG("*** react MessageSizeEvt ***");

        auto processSingleDigit = [this] {
            if (!isdigit(msgChunk.front())) {
                msgChunk.pop_front();
                transit<StateMessageType>();
            }
            else {
                msgPayloadSizeStr.push_back(msgChunk.front());
                msgChunk.pop_front();
                msgSizeBytesToRead--;
            }
        };

        if (msgSizeBytesToRead > msgChunk.size()) {
            while (!msgChunk.empty()) {
                processSingleDigit();
            }
            LOG_DEBUG("serial data buff empty, wait for new data");
            return;
        }
        else {
            while (msgSizeBytesToRead) {
                processSingleDigit();
            }
            msgPayloadSize = stoi(msgPayloadSizeStr);

            auto msgPayloadMalloc = [msg] {
                if (msgType == parserutils::message::Type::endpoint)
                    msgPayload.reserve(msgPayloadSize);
                else if (msgType == parserutils::message::Type::rawData)
                    send_event(RawDataMallocEvt(msg.ownerService));
            };
            transit<StateMessagePayload>(msgPayloadMalloc);
        }
    };
};

// ----------------------------------------------------------------------------
// State: CommandMessagePayload
//

class StateMessagePayload : public ParserFsm
{
    void react(MessageDataEvt const &msg) override
    {
        LOG_DEBUG("*** react MessagePayloadEvt ***");

        auto getSingleByte = [] {
            msgPayload.push_back(msgChunk.front());
            msgChunk.pop_front();
            msgPayloadSize--;
        };

        if (msgPayloadSize > msgChunk.size()) {
            msgPayload.reserve(msgPayload.size() + msgChunk.size());

            while (!msgChunk.empty()) {
                getSingleByte();
            }

            if (msgType == parserutils::message::Type::rawData) {
                send_event(RawDataEvt(msg.ownerService)); // invoke raw data fsm to let it store payload chunk to emmc
                msgPayload.clear();
            }
            LOG_DEBUG("serial data buff empty, wait for new data");
            return;
        }
        else {
            msgPayload.reserve(msgPayload.size() + msgPayloadSize);

            while (msgPayloadSize) {
                getSingleByte();
            }

            auto runHandler = [msg] {
                if (msgType == parserutils::message::Type::endpoint) {
                    send_event(EndpointEvt(msg.ownerService));
                }
                else if (msgType == parserutils::message::Type::rawData) {
                    send_event(RawDataEvt(msg.ownerService)); // invoke raw data fsm to let it store pyaload chunk to
                                                              // emmc and begin its own operations
                }
            };

            transit<StateMessageType>(runHandler);
        }
    };
};

// ----------------------------------------------------------------------------
// Base state: default implementations
//

void ParserFsm::react(MessageDataEvt const &)
{
    LOG_DEBUG("MessageDataEvt ignored");
}

std::list<int> ParserFsm::msgChunk;
parserutils::message::Type ParserFsm::msgType;
size_t ParserFsm::msgSizeBytesToRead;
std::string ParserFsm::msgPayloadSizeStr;
uint32_t ParserFsm::msgPayloadSize;
std::string ParserFsm::msgPayload;

// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(ParserFsm, StateMessageType)
