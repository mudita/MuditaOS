// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "ATFactory.hpp"
#include "cmd/CSCA.hpp"

namespace at
{
    using namespace std::chrono_literals;

    std::initializer_list<std::pair<const AT, const Cmd>> initializer = {
        {AT::AT, {"AT", 100ms}},
        {AT::ECHO_OFF, {"ATE0"}},
        {AT::FACTORY_RESET, {"AT&F"}},
        {AT::SW_INFO, {"ATI\r", default_doc_timeout}},
        {AT::FLOW_CTRL_ON, {"AT+IFC=2,2\r\n", 500ms}},
        {AT::FLOW_CTRL_OFF, {"AT+IFC=0,0", 500ms}},
        {AT::URC_NOTIF_CHANNEL, {"AT+QCFG=\"cmux/urcport\",1"}},
        {AT::RI_PIN_AUTO_CALL, {"AT+QCFG=\"urc/ri/ring\",\"auto\""}},
        {AT::RI_PIN_OFF_CALL, {"AT+QCFG=\"urc/ri/ring\",\"off\""}},
        {AT::RI_PIN_PULSE_SMS, {"AT+QCFG=\"urc/ri/smsincoming\",\"pulse\",450"}},
        {AT::RI_PIN_OFF_SMS, {"AT+QCFG=\"urc/ri/smsincoming\",\"off\""}},
        {AT::RI_PIN_PULSE_OTHER, {"AT+QCFG=\"urc/ri/other\",\"pulse\""}},
        {AT::URC_DELAY_ON, {"AT+QCFG=\"urc/delay\",1"}},
        {AT::URC_UART1, {"AT+QURCCFG=\"urcport\",\"uart1\""}},
        {AT::AT_PIN_READY_LOGIC, {"AT+QCFG=\"apready\",1,1,200"}},
        {AT::URC_NOTIF_SIGNAL, {"AT+QINDCFG=\"csq\",1"}},
        {AT::CRC_ON, {"AT+CRC=1"}},
        {AT::CALLER_NUMBER_PRESENTATION, {"AT+CLIP=1", 18000ms}},
        {AT::SMS_TEXT_FORMAT, {"AT+CMGF=1"}},
        {AT::SMS_UCSC2, {"AT+CSCS=\"UCS2\""}},
        {AT::SMS_GSM, {"AT+CSCS=\"GSM\""}},
        {AT::QSCLK_ON, {"AT+QSCLK=1", 3000ms}},
        {AT::QDAI, {"AT+QDAI?"}},
        {AT::QDAI_INIT, {"AT+QDAI=1,0,0,3,0,1,1,1"}},
        {AT::SET_URC_CHANNEL, {"AT+QCFG=\"cmux/urcport\",2", default_doc_timeout}},
        {AT::CSQ, {"AT+CSQ", default_doc_timeout}},
        {AT::CLCC, {"AT+CLCC", default_doc_timeout}},
        {AT::CMGD, {"AT+CMGD=", default_doc_timeout}},
        {AT::CNUM, {"AT+CNUM"}},
        {AT::CIMI, {"AT+CIMI"}},
        {AT::QCMGR, {"AT+QCMGR=", 2000ms}},
        {AT::ATH, {"ATH", 90000ms}},
        {AT::ATA, {"ATA", 90000ms}},
        {AT::ATD, {"ATD"}},
        {AT::IPR, {"AT+IPR="}},
        {AT::CMUX, {"AT+CMUX="}},
        {AT::CFUN, {"AT+CFUN=", default_long_doc_timeout}},
        {AT::CFUN_RESET, {"AT+CFUN=1,1", default_long_doc_timeout}},
        {AT::CFUN_MIN_FUNCTIONALITY, {"AT+CFUN=0", default_long_doc_timeout}},
        {AT::CFUN_FULL_FUNCTIONALITY, {"AT+CFUN=1", default_long_doc_timeout}},
        {AT::CFUN_DISABLE_TRANSMITTING, {"AT+CFUN=4", default_long_doc_timeout}},
        {AT::CMGS, {"AT+CMGS=\"", 120s}},   // verified in docs
        {AT::QCMGS, {"AT+QCMGS=\"", 120s}}, // verified in docs
        {AT::CREG, {"AT+CREG?", default_doc_timeout}},
        {AT::QNWINFO, {"AT+QNWINFO"}},
        {AT::COPS, {"AT+COPS", 180000ms}},
        {AT::QSIMSTAT, {"AT+QSIMSTAT?"}},
        {AT::SIM_DET, {"AT+QSIMDET?"}},
        {AT::SIM_DET_ON, {"AT+QSIMDET=1,0"}},
        {AT::SIMSTAT_ON, {"AT+QSIMSTAT=1"}},
        {AT::SET_SCANMODE, {"AT+QCFG=\"nwscanmode\","}},
        {AT::GET_SCANMODE, {"AT+QCFG=\"nwscanmode\""}},
        {AT::QGMR, {"AT+QGMR"}},
        {AT::STORE_SETTINGS_ATW, {"AT&W"}},
        {AT::CEER, {"AT+CEER"}},
        {AT::QIGETERROR, {"AT+QIGETERROR"}},
        {AT::VTS, {"AT+VTS="}},
        {AT::QLDTMF, {"AT+QLDTMF=1,"}},
        {AT::CUSD_OPEN_SESSION, {"AT+CUSD=1"}},
        {AT::CUSD_CLOSE_SESSION, {"AT+CUSD=2"}},
        {AT::CUSD_SEND, {"AT+CUSD=1,"}},
        {AT::SET_SMS_STORAGE, {"AT+CPMS=\"SM\",\"SM\",\"SM\"", 300ms}},
        {AT::CPIN, {"AT+CPIN=", default_timeout}},
        {AT::GET_CPIN, {"AT+CPIN?", default_timeout}},
        {AT::QPINC, {"AT+QPINC=", default_timeout}},
        {AT::CLCK, {"AT+CLCK=", default_timeout}},
        {AT::CPWD, {"AT+CPWD=", default_timeout}},
        {AT::ENABLE_TIME_ZONE_UPDATE, {"AT+CTZU=3"}},
        {AT::SET_TIME_ZONE_REPORTING, {"AT+CTZR=2"}},
        {AT::DISABLE_TIME_ZONE_UPDATE, {"AT+CTZU=0"}},
        {AT::DISABLE_TIME_ZONE_REPORTING, {"AT+CTZR=0"}},
        {AT::ENABLE_NETWORK_REGISTRATION_URC, {"AT+CREG=2"}},
        {AT::SET_SMS_TEXT_MODE_UCS2, {"AT+CSMP=17,167,0,8"}},
        {AT::LIST_MESSAGES, {"AT+CMGL=\"ALL\"", default_doc_timeout}},
        {AT::GET_IMEI, {"AT+GSN", default_doc_timeout}},
        {AT::CCFC, {"AT+CCFC=", default_doc_timeout}},
        {AT::CCWA, {"AT+CCWA=", default_doc_timeout}},
        {AT::CCWA_GET, {"AT+CCWA?", default_doc_timeout}},
        {AT::CHLD, {"AT+CHLD=\"", default_doc_timeout}},
        {AT::CLIP, {"AT+CLIP=", default_long_doc_timeout}},
        {AT::CLIP_GET, {"AT+CLIP?", default_long_doc_timeout}},
        {AT::CLIR, {"AT+CLIR=", default_long_doc_timeout}},
        {AT::CLIR_GET, {"AT+CLIR?", default_long_doc_timeout}},
        {AT::CLIR_RESET, {"AT+CLIR=0", default_long_doc_timeout}},
        {AT::CLIR_ENABLE, {"AT+CLIR=1", default_long_doc_timeout}},
        {AT::CLIR_DISABLE, {"AT+CLIR=2", default_long_doc_timeout}},
        {AT::COLP, {"AT+COLP", default_long_doc_timeout}},
        {AT::COLP_GET, {"AT+COLP?", default_long_doc_timeout}},
        {AT::COLP_ENABLE, {"AT+COLP=1", default_long_doc_timeout}},
        {AT::COLP_DISABLE, {"AT+COLP=0", default_long_doc_timeout}},
        {AT::CSSN, {"AT+CSSN=\"", default_doc_timeout}},
        {AT::QICSGP, {"AT+QICSGP", default_timeout}},
        {AT::QIACT, {"AT+QIACT", 150000ms}},
        {AT::QIDEACT, {"AT+QIDEACT", 40000ms}},
        {AT::QRXGAIN, {"AT+QRXGAIN=40000", default_timeout}},
        {AT::CLVL, {"AT+CLVL=3", default_timeout}},
        {AT::QMIC, {"AT+QMIC=15000,15000", default_timeout}},
        {AT::QEEC, {"AT+QEEC=", 3000ms}},
        {AT::QNVFR, {"AT+QNVFR=", default_long_doc_timeout}},
        {AT::QNVFW, {"AT+QNVFW=", default_long_doc_timeout}},
        {AT::QMBNCFG, {"AT+QMBNCFG=", default_long_doc_timeout}},
        {AT::QCFG_IMS, {"AT+QCFG=\"ims\"", default_timeout}}};

    auto factory(AT at) -> const Cmd &
    {
        static auto fact = std::map<AT, const Cmd>(initializer);
        if (fact.count(at) != 0u) {
            return fact.at(at);
        }
        LOG_ERROR("no such at command defined: %d", static_cast<int>(at));
        return fact.at(AT::AT);
    }
} // namespace at
