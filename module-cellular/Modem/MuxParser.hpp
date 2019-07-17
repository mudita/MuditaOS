
/*
 * @file MuxParser.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 17.07.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_MUXPARSER_HPP
#define PUREPHONE_MUXPARSER_HPP

#include <memory>
#include <optional>

namespace bsp{
    class Cellular;
}

class GSM0710Buffer;
class GSM0710Frame;
class MuxDaemon;
class InOutSerialWorker;

class MuxParser {
public:

    static std::optional<std::unique_ptr<MuxParser>> Create(MuxDaemon* mux,InOutSerialWorker* inOutSerial,bsp::Cellular* cellular);

    MuxParser(MuxDaemon* mux,InOutSerialWorker* inOutSerial,bsp::Cellular* cellular);


    int ProcessNewData();

    int ReadIncomingData();
    int ExtractFrames();
    int HandleCtrlChannelCommands(GSM0710Frame *frame);

private:

    MuxDaemon* mux;
    InOutSerialWorker* inOutSerialWorker;
    bsp::Cellular* cellular;
    std::unique_ptr<GSM0710Buffer> inputBuffer=nullptr;

    bool isInitialized = false;

};


#endif //PUREPHONE_MUXPARSER_HPP
