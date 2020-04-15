/*
 *  @file InternetServiceAPI.hpp
 *  @author Łukasz Skrzypczak (lukasz.skrzypczak@mudita.com)
 *  @date 16.09.19
 *  @brief
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */

#ifndef PUREPHONE_INTERNETSERVICEAPI_HPP
#define PUREPHONE_CELLULARSERVICEAPI_HPP

#include <service-internet/messages/InternetMessage.hpp>
#include <service-cellular/messages/CellularMessage.hpp>

class Service;

class InternetServiceAPI
{
  public:
    static bool Connect(sys::Service *serv);
    static bool Disconnect(sys::Service *serv);
    static std::string HTTPGET(sys::Service *serv, const std::string &url, const std::string &request);
    static std::string HTTPPOST(sys::Service *serv,
                                const std::string &url,
                                const std::string &request,
                                const std::string &audio);

    enum class encodings
    {
        encUNSPECIFIED,
        encLINEAR16,
        encFLAC,
        encMULAW,
        encAMR,
        encAMR_WB,
        encOGG,
        encSPEEX
    };

    enum class languages
    {
        langEN,
        langPL,
        langES
    };

    static std::string json(encodings encoding, int samplerate, languages language, uint8_t *buffer, uint32_t length);
};

#endif // PUREPHONE_INTERNETSERVICEAPI_HPP
