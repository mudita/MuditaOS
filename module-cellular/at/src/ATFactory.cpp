// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ATFactory.hpp"
#include "cmd/CSCA.hpp"
#include <memory>

namespace at
{
    using namespace std::chrono_literals;
    using namespace std::string_literals;
    namespace
    {
        std::map<const AT, std::shared_ptr<const Cmd>> g_cmds_map{
            {AT::AT, std::make_shared<Cmd>("AT")},
            {AT::ECHO_OFF, std::make_shared<Cmd>("ATE0")},
            {AT::FACTORY_RESET, std::make_shared<Cmd>("AT&F")},
            {AT::SW_INFO, std::make_shared<Cmd>("ATI\r")},
            {AT::FLOW_CTRL_ON, std::make_shared<Cmd>("AT+IFC=2,2\r\n")},
            {AT::FLOW_CTRL_OFF, std::make_shared<Cmd>("AT+IFC=0,0")},
            {AT::URC_NOTIF_CHANNEL, std::make_shared<Cmd>("AT+QCFG=\"cmux/urcport\",1")},
            {AT::RI_PIN_AUTO_CALL, std::make_shared<Cmd>("AT+QCFG=\"urc/ri/ring\",\"auto\"")},
            {AT::RI_PIN_OFF_CALL, std::make_shared<Cmd>("AT+QCFG=\"urc/ri/ring\",\"off\"")},
            {AT::RI_PIN_PULSE_SMS, std::make_shared<Cmd>("AT+QCFG=\"urc/ri/smsincoming\",\"pulse\",450")},
            {AT::RI_PIN_OFF_SMS, std::make_shared<Cmd>("AT+QCFG=\"urc/ri/smsincoming\",\"off\"")},
            {AT::RI_PIN_PULSE_OTHER, std::make_shared<Cmd>("AT+QCFG=\"urc/ri/other\",\"pulse\"")},
            {AT::URC_DELAY_ON, std::make_shared<Cmd>("AT+QCFG=\"urc/delay\",1")},
            {AT::URC_UART1, std::make_shared<Cmd>("AT+QURCCFG=\"urcport\",\"uart1\"")},
            {AT::AT_PIN_READY_LOGIC, std::make_shared<Cmd>("AT+QCFG=\"apready\",1,1,200")},
            {AT::CSQ_URC_ON, std::make_shared<Cmd>("AT+QINDCFG=\"csq\",1")},
            {AT::CSQ_URC_OFF, std::make_shared<Cmd>("AT+QINDCFG=\"csq\",0")},
            {AT::CRC_ON, std::make_shared<Cmd>("AT+CRC=1")},
            {AT::CALLER_NUMBER_PRESENTATION, std::make_shared<Cmd>("AT+CLIP=1", default_long_timeout)},
            {AT::SMS_TEXT_FORMAT, std::make_shared<Cmd>("AT+CMGF=1")},
            {AT::SMS_UCSC2, std::make_shared<Cmd>("AT+CSCS=\"UCS2\"")},
            {AT::SMS_GSM, std::make_shared<Cmd>("AT+CSCS=\"GSM\"")},
            {AT::QSCLK_ON, std::make_shared<Cmd>("AT+QSCLK=1")},
            {AT::QDAI, std::make_shared<Cmd>("AT+QDAI?")},
            {AT::QDAI_INIT, std::make_shared<Cmd>("AT+QDAI=1,0,0,3,0,1,1,1")},
            {AT::SET_URC_CHANNEL, std::make_shared<Cmd>("AT+QCFG=\"cmux/urcport\",2")},
            {AT::CSQ, std::make_shared<Cmd>("AT+CSQ")},
            {AT::CLCC, std::make_shared<Cmd>("AT+CLCC")},
            {AT::CMGD, std::make_shared<Cmd>("AT+CMGD=")},
            {AT::CNUM, std::make_shared<Cmd>("AT+CNUM")},
            {AT::CIMI, std::make_shared<Cmd>("AT+CIMI")},
            {AT::QCMGR, std::make_shared<Cmd>("AT+QCMGR=", 180s)},
            {AT::ATH, std::make_shared<Cmd>("ATH", 100s)},
            {AT::CHUP, std::make_shared<Cmd>("AT+CHUP", 100s)},
            {AT::QHUP_BUSY, std::make_shared<Cmd>("AT+QHUP=17", 100s)},
            {AT::ATA, std::make_shared<Cmd>("ATA", 100s)},
            {AT::ATD, std::make_shared<Cmd>("ATD", 6s)},
            {AT::IPR, std::make_shared<Cmd>("AT+IPR=")},
            {AT::CMUX, std::make_shared<Cmd>("AT+CMUX=")},
            {AT::CFUN, std::make_shared<Cmd>("AT+CFUN=", default_long_timeout)},
            {AT::CFUN_RESET, std::make_shared<Cmd>("AT+CFUN=1,1", default_long_timeout)},
            {AT::CFUN_MIN_FUNCTIONALITY, std::make_shared<Cmd>("AT+CFUN=0", default_long_timeout)},
            {AT::CFUN_FULL_FUNCTIONALITY, std::make_shared<Cmd>("AT+CFUN=1", default_long_timeout)},
            {AT::CFUN_DISABLE_TRANSMITTING, std::make_shared<Cmd>("AT+CFUN=4", default_long_timeout)},
            {AT::CMGS, std::make_shared<Cmd>("AT+CMGS=\"", 180s)}, //
            {AT::QCMGS, std::make_shared<Cmd>("AT+QCMGS=\"", 180s)},
            {AT::CREG, std::make_shared<Cmd>("AT+CREG?")},
            {AT::QNWINFO, std::make_shared<Cmd>("AT+QNWINFO")},
            {AT::COPS, std::make_shared<Cmd>("AT+COPS", 200s)},
            {AT::QSIMSTAT, std::make_shared<Cmd>("AT+QSIMSTAT?")},
            {AT::SIM_DET, std::make_shared<Cmd>("AT+QSIMDET?")},
            {AT::SIM_DET_ON, std::make_shared<Cmd>("AT+QSIMDET=1,0")},
            {AT::SIMSTAT_ON, std::make_shared<Cmd>("AT+QSIMSTAT=1")},
            {AT::SET_DEFAULT_SCANMODE, std::make_shared<Cmd>("AT+QCFG=\"nwscanmode\",1,1")},
            {AT::QUERY_SERVING_CELL, std::make_shared<Cmd>("AT+QENG=\"servingcell\"")},
            {AT::SET_DEFAULT_SERVICEDOMAIN, std::make_shared<Cmd>("AT+QCFG=\"servicedomain\",2")},
            {AT::SET_SERVICEDOMAIN, std::make_shared<Cmd>("AT+QCFG=\"servicedomain\",")},
            {AT::GET_SERVICEDOMAIN, std::make_shared<Cmd>("AT+QCFG=\"servicedomain\"")},
            {AT::QGMR, std::make_shared<Cmd>("AT+QGMR")},
            {AT::STORE_SETTINGS_ATW, std::make_shared<Cmd>("AT&W")},
            {AT::CEER, std::make_shared<Cmd>("AT+CEER", 1s)},
            {AT::QIGETERROR, std::make_shared<Cmd>("AT+QIGETERROR")},
            {AT::VTS, std::make_shared<Cmd>("AT+VTS=", default_long_timeout)},
            {AT::QLDTMF, std::make_shared<Cmd>("AT+QLDTMF=1,")},
            {AT::CUSD_OPEN_SESSION, std::make_shared<Cmd>("AT+CUSD=1", 150s)},
            {AT::CUSD_CLOSE_SESSION, std::make_shared<Cmd>("AT+CUSD=2", 150s)},
            {AT::CUSD_SEND, std::make_shared<Cmd>("AT+CUSD=1,", 150s)},
            {AT::SET_SMS_STORAGE, std::make_shared<Cmd>("AT+CPMS=\"SM\",\"SM\",\"SM\"")},
            {AT::CPIN, std::make_shared<Cmd>("AT+CPIN=", default_long_timeout)},
            {AT::GET_CPIN, std::make_shared<Cmd>("AT+CPIN?", default_long_timeout)},
            {AT::QPINC, std::make_shared<Cmd>("AT+QPINC=", default_long_timeout)},
            {AT::CLCK, std::make_shared<Cmd>("AT+CLCK=", default_long_timeout)},
            {AT::CPWD, std::make_shared<Cmd>("AT+CPWD=", default_long_timeout)},
            {AT::ENABLE_TIME_ZONE_UPDATE, std::make_shared<Cmd>("AT+CTZU=3")},
            {AT::SET_TIME_ZONE_REPORTING, std::make_shared<Cmd>("AT+CTZR=2")},
            {AT::DISABLE_TIME_ZONE_UPDATE, std::make_shared<Cmd>("AT+CTZU=0")},
            {AT::DISABLE_TIME_ZONE_REPORTING, std::make_shared<Cmd>("AT+CTZR=0")},
            {AT::ENABLE_NETWORK_REGISTRATION_URC, std::make_shared<Cmd>("AT+CREG=2")},
            {AT::SET_SMS_TEXT_MODE_UCS2, std::make_shared<Cmd>("AT+CSMP=17,167,0,8")},
            {AT::LIST_MESSAGES, std::make_shared<Cmd>("AT+CMGL=\"ALL\"")},
            {AT::GET_IMEI, std::make_shared<Cmd>("AT+GSN")},
            {AT::CCFC, std::make_shared<Cmd>("AT+CCFC=")},
            {AT::CCWA, std::make_shared<Cmd>("AT+CCWA=")},
            {AT::CCWA_GET, std::make_shared<Cmd>("AT+CCWA?")},
            {AT::CHLD, std::make_shared<Cmd>("AT+CHLD=\"")},
            {AT::CLIP, std::make_shared<Cmd>("AT+CLIP=", default_long_timeout)},
            {AT::CLIP_GET, std::make_shared<Cmd>("AT+CLIP?", default_long_timeout)},
            {AT::CLIR, std::make_shared<Cmd>("AT+CLIR=", default_long_timeout)},
            {AT::CLIR_GET, std::make_shared<Cmd>("AT+CLIR?", default_long_timeout)},
            {AT::CLIR_RESET, std::make_shared<Cmd>("AT+CLIR=0", default_long_timeout)},
            {AT::CLIR_ENABLE, std::make_shared<Cmd>("AT+CLIR=1", default_long_timeout)},
            {AT::CLIR_DISABLE, std::make_shared<Cmd>("AT+CLIR=2", default_long_timeout)},
            {AT::COLP, std::make_shared<Cmd>("AT+COLP", default_long_timeout)},
            {AT::COLP_GET, std::make_shared<Cmd>("AT+COLP?", default_long_timeout)},
            {AT::COLP_ENABLE, std::make_shared<Cmd>("AT+COLP=1", default_long_timeout)},
            {AT::COLP_DISABLE, std::make_shared<Cmd>("AT+COLP=0", default_long_timeout)},
            {AT::CSSN, std::make_shared<Cmd>("AT+CSSN=\"")},
            {AT::QICSGP, std::make_shared<Cmd>("AT+QICSGP")},
            {AT::QIACT, std::make_shared<Cmd>("AT+QIACT", 150s)},
            {AT::QIDEACT, std::make_shared<Cmd>("AT+QIDEACT", 40s)},
            {AT::QRXGAIN, std::make_shared<Cmd>("AT+QRXGAIN=40000")},
            {AT::CLVL, std::make_shared<Cmd>("AT+CLVL=3")},
            {AT::QMIC, std::make_shared<Cmd>("AT+QMIC=15000,15000")},
            {AT::QEEC, std::make_shared<Cmd>("AT+QEEC=")},
            {AT::QNVFR, std::make_shared<Cmd>("AT+QNVFR=", default_long_timeout)},
            {AT::QNVFW, std::make_shared<Cmd>("AT+QNVFW=", default_long_timeout)},
            {AT::QMBNCFG, std::make_shared<Cmd>("AT+QMBNCFG=", default_long_timeout)},
            {AT::QCFG_IMS, std::make_shared<Cmd>("AT+QCFG=\"ims\"")},
            {AT::RING_URC_ON, std::make_shared<Cmd>("AT+QINDCFG=\"ring\",1")},
            {AT::RING_URC_OFF, std::make_shared<Cmd>("AT+QINDCFG=\"ring\",0")},
            {AT::ACT_URC_OFF, std::make_shared<Cmd>("AT+QINDCFG=\"act\",0")},
            {AT::ACT_URC_ON, std::make_shared<Cmd>("AT+QINDCFG=\"act\",1")},
            {AT::SMS_URC_ON, std::make_shared<Cmd>("AT+QINDCFG=\"smsincoming\",1")},
            {AT::SMS_URC_OFF, std::make_shared<Cmd>("AT+QINDCFG=\"smsincoming\",0")},
            {AT::SET_RNDIS, std::make_shared<Cmd>("AT+QCFG=\"usbnet\",3")},
        };
    }
    auto factory(AT at) -> const Cmd &
    {
        if (g_cmds_map.count(at) != 0u) {
            assert(g_cmds_map.at(at));
            return *(g_cmds_map.at(at));
        }
        LOG_ERROR("no such at command defined: %d", static_cast<int>(at));
        assert(g_cmds_map.at(AT::AT));
        return *(g_cmds_map.at(AT::AT));
    }
} // namespace at
