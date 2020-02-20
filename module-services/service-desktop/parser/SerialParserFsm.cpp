#include "SerialParserFsm.hpp"
#include "Fsmlist.hpp"
#include "ParserUtils.hpp"
#include "log/log.hpp"

class StateMessageType;
class StateMessageSize;
class StateMessagePayload;

// ----------------------------------------------------------------------------
// State: MessageType
//

class StateMessageType
: public SerialParserFsm
{
    void entry() override
    {
        LOG_DEBUG("*** entry StateMessageType ***");
        //Restore initial state of all buffers and FSMs
        msgType = static_cast<uint8_t>(parserutils::message::type::invalid);
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
        msgType = static_cast<uint8_t>(msgChunk.front());
        msgChunk.pop_front();

        switch (msgType)
        {
        case parserutils::message::type::endpoint:
        case parserutils::message::type::rawData:
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

class StateMessageSize
: public SerialParserFsm
{
    void react(MessageDataEvt const &) override
    {
        LOG_DEBUG("*** react MessageSizeEvt ***");

        auto processSingleDigit = [this] { 
              if(!isdigit(msgChunk.front()))
                {msgChunk.pop_front();
                transit<StateMessageType>();}
              else
                {msgPayloadSizeStr.push_back(msgChunk.front());
                msgChunk.pop_front();
                msgSizeBytesToRead--;}
            };
        
        if (msgSizeBytesToRead > msgChunk.size())
        {
            while (!msgChunk.empty())
            {
                processSingleDigit();
            }
            LOG_DEBUG("serial data buff empty, wait for new data");
            return;
        }
        else
        {
            while (msgSizeBytesToRead)
            {
                processSingleDigit();
            }
            msgPayloadSize = stoi(msgPayloadSizeStr);

            auto msgPayloadMalloc = [this] {
                if (msgType == parserutils::message::type::endpoint)
                    msgPayload.reserve(msgPayloadSize);
                else if (msgType == parserutils::message::type::rawData)
                    send_event(RawDataMallocEvt());
            };
            transit<StateMessagePayload>(msgPayloadMalloc);
        }
    };
};


// ----------------------------------------------------------------------------
// State: CommandMessagePayload
//

class StateMessagePayload
: public SerialParserFsm
{
    void react(MessageDataEvt const &) override
    {
        LOG_DEBUG("*** react MessagePayloadEvt ***");

        auto getSingleByte = [this] { 
            msgPayload.push_back(msgChunk.front());
            msgChunk.pop_front();
            msgPayloadSize--;
        };

        if (msgPayloadSize > msgChunk.size())
        {
            if (msgType == parserutils::message::type::rawData)
            {
                msgPayload.reserve(msgChunk.size());
            }

            while (!msgChunk.empty())
            {
                getSingleByte();
            }
            if (msgType == parserutils::message::type::rawData)
            {
                send_event(RawDataEvt()); //invoke raw data fsm to let it store pyaload chunk to emmc
                msgPayload.clear();
            }
            LOG_DEBUG("serial data buff empty, wait for new data");
            return;
        }
        else
        {
            if (msgType == parserutils::message::type::rawData)
            {
                msgPayload.reserve(msgPayloadSize);
            }

            while (msgPayloadSize)
            {
                getSingleByte();
            }

            auto runHandler = [this] {
                if (msgType == parserutils::message::type::endpoint)
                    send_event(EndpointEvt());
                else if (msgType == parserutils::message::type::rawData)
                    send_event(RawDataEvt()); // invoke raw data fsm to let it store pyaload chunk to emmc and begin its own operations
            };

            transit<StateMessageType>(runHandler);
        }
    };
};


// ----------------------------------------------------------------------------
// Base state: default implementations
//

void SerialParserFsm::react(MessageDataEvt const &)
{
    LOG_DEBUG("MessageDataEvt ignored");
}

std::list<int> SerialParserFsm::msgChunk;
uint8_t SerialParserFsm::msgType;
size_t SerialParserFsm::msgSizeBytesToRead;
std::string SerialParserFsm::msgPayloadSizeStr;
uint32_t SerialParserFsm::msgPayloadSize;
std::string SerialParserFsm::msgPayload;

// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(SerialParserFsm, StateMessageType)
