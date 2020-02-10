#include "Fsmlist.hpp"
#include "log/log.hpp"
#include "SerialParserFsm.hpp"



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
        msgType = MessageType::Invalid;
        msgSizeBytesToRead = 9;
        msgPayloadSizeStr.erase();
        msgPayloadSize = 0;
        msgPayload.clear();
        EndpointFsm::reset();
        RawDataFsm::reset();
    };

    void react(MessageDataEvt const &) override
    {
        LOG_DEBUG("*** react MessageTypeEvt ***");
        msgType = static_cast<MessageType>(msgChunk.front()); //get first byte of message
        msgChunk.pop_front();
        
        LOG_DEBUG("msgType: %d", msgType);
        
        switch (msgType)
        {
            case MessageType::Endpoint:
            case MessageType::RawData:
                transit<StateMessageSize>();
                break;
            default:
                LOG_ERROR("Invalid msg type!");
                if (msgChunk.empty())
                {
                    return;
                }
                else
                {
                    send_event(MessageDataEvt());
                }
                //alternative: transit to StateMessageType
                //it will erase all buffs and prevent stackoverflow in some cases, but we loose some data
        }
    };

    void exit() override
    {
        LOG_DEBUG("*** exit StateMessageType ***");
        //quit parser after reaching end of buff
        if (msgChunk.empty())
        {
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
    void entry() override
    {
        LOG_DEBUG("*** entry StateMessageSize ***");
        send_event(MessageDataEvt());
    };

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
            LOG_DEBUG("msgPayloadSize: %d", msgPayloadSize);

            auto msgPayloadMalloc = [this] { 
              if(msgType == MessageType::Endpoint)
                msgPayload.reserve(msgPayloadSize);
              else if(msgType == MessageType::RawData)
                send_event(RawDataMallocEvt());//send event for memory alloc on emmc
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
    void entry() override
    {
        LOG_DEBUG("*** entry StateMessagePayload ***");
        send_event(MessageDataEvt());
    };

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
            if (msgType == MessageType::RawData)
            {
                msgPayload.reserve(msgChunk.size());
            }

            while (!msgChunk.empty())
            {
                getSingleByte();
            }
            if (msgType == MessageType::RawData)
            {
                send_event(RawDataEvt()); //invoke raw data fsm to let it store pyaload chunk to emmc
                msgPayload.clear();
            }
            LOG_DEBUG("serial data buff empty, wait for new data");
            return;
        }
        else
        {
            if (msgType == MessageType::RawData)
            {
                msgPayload.reserve(msgPayloadSize);
            }

            while (msgPayloadSize)
            {
                getSingleByte();
            }

            auto runHandler = [this] { 
              if(msgType == MessageType::Endpoint)
                send_event(EndpointEvt()); //invoke endpoint fsm
              else if(msgType == MessageType::RawData)
                send_event(RawDataEvt()); //invoke raw data fsm to let it store pyaload chunk to emmc and begin its own operations
            };

            transit<StateMessageType>(runHandler);
        }
    };
};


// ----------------------------------------------------------------------------
// Base state: default implementations
//

void SerialParserFsm::react(MessageDataEvt const &) {
  LOG_DEBUG("MessageDataEvt ignored");
}

std::list<int>                  SerialParserFsm::msgChunk;
SerialParserFsm::MessageType    SerialParserFsm::msgType;
size_t                          SerialParserFsm::msgSizeBytesToRead;
std::string                     SerialParserFsm::msgPayloadSizeStr;
uint32_t                        SerialParserFsm::msgPayloadSize;
std::string                     SerialParserFsm::msgPayload;

// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(SerialParserFsm, StateMessageType)
