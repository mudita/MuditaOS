/**
 * Project Untitled
 */


#include "TS0710_DATA.h"
#include "TS0710_Frame.h"

#if defined (__cplusplus)
    extern "C"{
#endif
//#include "uart.h"
#if defined(__cplusplus)
}
#endif

/**
 * TS0710_DATA implementation
 */
TS0710_DATA::TS0710_DATA(DLCI_t DLCI, DLC_ESTABL_SystemParameters_t sysParams, std::vector<uint8_t> User_data, bsp::Cellular *cellular) {
    pv_cellular = cellular;

    request(DLCI, sysParams, User_data);
}

/**
 * @param DLCI
 * @param User_data
 */
void TS0710_DATA::request(DLCI_t DLCI, DLC_ESTABL_SystemParameters_t sysParams, std::vector<uint8_t> User_data) {
    /*
    TE sends AT command "ATI<CR>" through DLC 1
        TE→MS   F9 05 EF 09 41 54 49 0D 58 F9   UIH Frame
    MS feeds back through DLC 1
        TE←MS   F9 05 EF 09 41 54 49 0D 58 F9
                F9 05 EF 9B 0D 0A 53 49 4D 43
                4F 4D 5F 4C 74 64 0D 0A 53 49
                4D 43 4F 4D 5F 53 49 4D 33 30
                30 0D 0A 52 65 76 69 73 69 6F 6E
                3A 53 49 4D 33 30 30 4D 33 32 28
                53 50 41 4E 53 49 4F 4E 29 5F 56
                31 30 2E 30 2E 38 5F 42 55 49
                4C 44 30 33 0D 0A 0D 0A 47 F9   UIH Frame
*/
    TS0710_Frame::frame_t frame;
    frame.Address = static_cast<uint8_t>(DLCI << 2) /*| (1 << 1)*/;  //set C/R = 1 - command
    frame.Control = static_cast<uint8_t>(TypeOfFrame_e::UIH);
    
    if (User_data.size() < static_cast<size_t>(sysParams.MaxFrameSize)) {
        frame.data = User_data;
        TS0710_Frame frame_c(frame);
        //UartSend(frame_c.getSerData().data(), frame_c.getSerData().size());
        pv_cellular->Write(static_cast<void*>(frame_c.getSerData().data()), frame_c.getSerData().size());
    }
    else {  //if data size > max frame size
        int dataLeft = User_data.size();
        std::vector<uint8_t>::iterator i = User_data.begin();
        uint32_t parts = User_data.size() / sysParams.MaxFrameSize + 1; //add reminder
        LOG_DEBUG("SENDING %i parts", parts);
        while (parts--) {
            std::vector<uint8_t>::iterator last = i + (dataLeft < sysParams.MaxFrameSize ? dataLeft : sysParams.MaxFrameSize);  //distinguish reminder
            frame.data = std::vector<uint8_t>(i, last);
            TS0710_Frame frame_c(frame);
            //UartSend(frame_c.getSerData().data(), frame_c.getSerData().size());
            //while(!pv_cellular->GetSendingAllowed());
            pv_cellular->Write(static_cast<void*>(frame_c.getSerData().data()), frame_c.getSerData().size());
            //vTaskDelay(1);
            dataLeft -= (dataLeft < sysParams.MaxFrameSize ? dataLeft : sysParams.MaxFrameSize);
        }
    }
    
}

/**
 * @param DLCI
 * @param User_data
 */
void TS0710_DATA::indication(DLCI_t DLCI, std::vector<uint8_t> User_data) {

}