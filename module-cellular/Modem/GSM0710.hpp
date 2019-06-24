
/*
 * @file GSM0710.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_GSM0710_HPP
#define PUREPHONE_GSM0710_HPP

#include <stdint.h>
#include <memory>

/*MUX defines */
enum class MuxDefines {
    GSM0710_FRAME_FLAG = 0xF9,// basic mode flag for frame start and end
    GSM0710_FRAME_ADV_FLAG = 0x7E,// advanced mode flag for frame start and end
    GSM0710_FRAME_ADV_ESC = 0x7D,// advanced mode escape symbol
    GSM0710_FRAME_ADV_ESC_COPML = 0x20,// advanced mode escape complement mask
    // bits: Poll/final, Command/Response, Extension
    GSM0710_PF = 0x10,//16
    GSM0710_CR = 0x02,//2
    GSM0710_EA = 0x01,//1
    // type of frames
    GSM0710_TYPE_SABM = 0x2F,//47 Set Asynchronous Balanced Mode
    GSM0710_TYPE_UA = 0x63,//99 Unnumbered Acknowledgement
    GSM0710_TYPE_DM = 0x0F,//15 Disconnected Mode
    GSM0710_TYPE_DISC = 0x43,//67 Disconnect
    GSM0710_TYPE_UIH = 0xEF,//239 Unnumbered information with header check
    GSM0710_TYPE_UI = 0x03,//3 Unnumbered Acknowledgement
    // control channel commands
    GSM0710_CONTROL_PN = (0x80 | GSM0710_EA),//?? DLC parameter negotiation
    GSM0710_CONTROL_CLD = (0xC0 | GSM0710_EA),//193 Multiplexer close down
    GSM0710_CONTROL_PSC = (0x40 | GSM0710_EA),//??? Power Saving Control
    GSM0710_CONTROL_TEST = (0x20 | GSM0710_EA),//33 Test Command
    GSM0710_CONTROL_MSC = (0xE0 | GSM0710_EA),//225 Modem Status Command
    GSM0710_CONTROL_NSC = (0x10 | GSM0710_EA),//17 Non Supported Command Response
    GSM0710_CONTROL_RPN = (0x90 | GSM0710_EA),//?? Remote Port Negotiation Command
    GSM0710_CONTROL_RLS = (0x50 | GSM0710_EA),//?? Remote Line Status Command
    GSM0710_CONTROL_SNC = (0xD0 | GSM0710_EA),//?? Service Negotiation Command
    // V.24 signals: flow control, ready to communicate, ring indicator,
    // data valid
    GSM0710_SIGNAL_FC = 0x02,
    GSM0710_SIGNAL_RTC = 0x04,
    GSM0710_SIGNAL_RTR = 0x08,
    GSM0710_SIGNAL_IC = 0x40,//64
    GSM0710_SIGNAL_DV = 0x80,//128
    GSM0710_SIGNAL_DTR = 0x04,
    GSM0710_SIGNAL_DSR = 0x04,
    GSM0710_SIGNAL_RTS = 0x08,
    GSM0710_SIGNAL_CTS = 0x08,
    GSM0710_SIGNAL_DCD = 0x80//128
};

struct GSM0710Frame {
    unsigned char channel;
    unsigned char control;
    int length;
    unsigned char *data;
};

class GSM0710Buffer {

    //reversed, 8-bit, poly=0x07
    static constexpr unsigned char crcTable[] = {

            0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75, 0x0E, 0x9F, 0xED,
            0x7C, 0x09, 0x98, 0xEA, 0x7B, 0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A,
            0xF8, 0x69, 0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67, 0x38,
            0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D, 0x36, 0xA7, 0xD5, 0x44,
            0x31, 0xA0, 0xD2, 0x43, 0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0,
            0x51, 0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F, 0x70, 0xE1,
            0x93, 0x02, 0x77, 0xE6, 0x94, 0x05, 0x7E, 0xEF, 0x9D, 0x0C, 0x79,
            0xE8, 0x9A, 0x0B, 0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19,
            0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17, 0x48, 0xD9, 0xAB,
            0x3A, 0x4F, 0xDE, 0xAC, 0x3D, 0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0,
            0xA2, 0x33, 0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21, 0x5A,
            0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F, 0xE0, 0x71, 0x03, 0x92,
            0xE7, 0x76, 0x04, 0x95, 0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A,
            0x9B, 0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89, 0xF2, 0x63,
            0x11, 0x80, 0xF5, 0x64, 0x16, 0x87, 0xD8, 0x49, 0x3B, 0xAA, 0xDF,
            0x4E, 0x3C, 0xAD, 0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
            0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1, 0xCA, 0x5B, 0x29,
            0xB8, 0xCD, 0x5C, 0x2E, 0xBF, 0x90, 0x01, 0x73, 0xE2, 0x97, 0x06,
            0x74, 0xE5, 0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB, 0x8C,
            0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9, 0x82, 0x13, 0x61, 0xF0,
            0x85, 0x14, 0x66, 0xF7, 0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C,
            0xDD, 0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3, 0xB4, 0x25,
            0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 0xBA, 0x2B, 0x59, 0xC8, 0xBD,
            0x2C, 0x5E, 0xCF,
    };

public:

    // some state
    // +CMUX=<mode>[,<subset>[,<port_speed>[,<N1>[,<T1>[,<N2>[,<T2>[,<T3>[,<k>]]]]]]]]
    const static int cmux_mode = 0;
    const static int cmux_subset = 0;
    const static int cmux_port_speed = 5; //115200 baud rate
    const static int cmux_port_speed_default = 5; //115200 baud rate
    const static int vir_ports = 3; /* number of virtual ports to create */
    const static int cmux_N1 = 127; //lots of modem only support short frame, like quectel 2G modules
    const static uint32_t cmux_FRAME = (cmux_N1 + 6); //lots of modem only support short frame, like quectel 2G modules
    const static uint32_t QUECTEL_CACHE_FRAMES = 20;
    const static uint32_t GSM0710_BUFFER_SIZE = (2 * QUECTEL_CACHE_FRAMES * cmux_FRAME);

    unsigned char data[GSM0710_BUFFER_SIZE];
    unsigned char *readp;
    unsigned char *writep;
    unsigned char *endp;
    unsigned int datacount;
    unsigned int max_count;
    int flag_found;// set if last character read was flag
    unsigned long received_count;
    unsigned long dropped_count;


    GSM0710Buffer() {
        writep = data;
        readp = data;
        endp = data + sizeof(data);
    }

    inline uint32_t GetFreeSpace() {
        return GSM0710_BUFFER_SIZE - datacount;
    }

    inline uint32_t GetDataLength() {
        return datacount;
    }

    inline void Inc() {
        readp++;
        datacount--;
        if (readp == endp) {
            readp = data;
        }
    }

    inline bool GSM0710_FRAME_IS(MuxDefines type,GSM0710Frame* frame){
        return (frame->control & ~static_cast<unsigned char>(MuxDefines::GSM0710_PF)) == static_cast<unsigned char>(type);
    }

    inline bool GSM0710_COMMAND_IS(MuxDefines type,unsigned char frame){
        return (frame & ~static_cast<unsigned char>(MuxDefines::GSM0710_CR)) == static_cast<unsigned char>(type);
    }

    static unsigned char frameCalcCRC(
            const unsigned char *input,
            int length);


    std::unique_ptr<GSM0710Frame> GetCompleteFrame(std::unique_ptr<GSM0710Frame> frame);

};

#endif //PUREPHONE_GSM0710_HPP
