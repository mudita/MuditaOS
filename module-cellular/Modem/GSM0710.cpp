
/*
 * @file GSM0710.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "GSM0710.hpp"

#include "log/log.hpp"

constexpr unsigned char GSM0710Buffer::crcTable[];


GSM0710Frame*  GSM0710Buffer::GetCompleteFrame(GSM0710Frame* frame) {
    int end;

    switch (state){

        case State ::SearchForNewFrame:
        {

            /*Find start flag*/
            while (!flag_found && GetDataLength() > 0)
            {
                if (*readp == static_cast<unsigned char>(MuxDefines ::GSM0710_FRAME_FLAG)) {
                    flag_found = 1;
                }
                Inc();
            }

            if (!flag_found)// no frame started
            {
                LOG_DEBUG("Leave. No start frame 0xf9 found in bytes stored in GSM0710 buffer");
                return nullptr;
            }

            state = State ::ParseFrameHeader;


        }

        case State ::ParseFrameHeader:
        {
            /*skip empty frames (this causes troubles if we're using DLC 62) - skipping frame start flags*/
            while (GetDataLength() > 0 && (*readp == static_cast<unsigned char>(MuxDefines ::GSM0710_FRAME_FLAG)))
            {
                Inc();
            }

            /* Okay, we're ready to analyze a proper frame header */
            if (datacount >= lengthNeeded) /* enough data stored for 0710 frame header+footer? */
            {
                currentFrame->channel = ((*readp & 252) >> 2); /*frame header address-byte read*/
                if (currentFrame->channel >= vir_ports) /* Field Sanity check if channel ID actually exists */
                {
                    LOG_WARN("Dropping frame: Corrupt! Channel Addr. field indicated %d, which does not exist",
                             currentFrame->channel);
                    flag_found = 0;
                    dropped_count++;
                    /* throw whole frame away*/
                    state = State::SearchForNewFrame;
                    lengthNeeded = 5;
                    fcs = 0xFF;
                    goto search_for_new_frame;
                }
                fcs = crcTable[fcs ^ *readp];
                Inc();
                lengthNeeded--;
                currentFrame->control = *readp; /*frame header type-byte read*/
                fcs = crcTable[fcs ^ *readp];
                Inc();
                lengthNeeded--;
                currentFrame->length = (*readp & 254) >> 1; /*Frame header 1st length-byte read*/
                fcs = crcTable[fcs ^ *readp];

                if ((*readp & 1) ==
                    0)/*if frame payload length byte extension bit not set, a 2nd length byte is in header*/
                {
                    //Current spec (version 7.1.0) states these kind of
                    //frames to be invalid Long lost of sync might be
                    //caused if we would expect a long frame because of an
                    //error in length field.
                    volatile uint32_t err =0;
                    Inc();
                    currentFrame->length += (*readp*128); /*Frame header 2nd length-byte read*/
                    fcs = crcTable[fcs ^ *readp];
                }

                lengthNeeded += currentFrame->length; /*length_needed : 1 length byte + payload + 1 fcs byte + 1 end frame flag */
                LOG_DEBUG("length_needed: %d, available in local_datacount: %d", lengthNeeded, datacount);

                if (currentFrame->length >
                    cmux_N1) /* Field Sanity check if payload is bigger than the max size negotiated in +CMUX */
                {
                    LOG_WARN("Dropping frame: Corrupt! Length field indicated %d. Max %d allowed", currentFrame->length,
                             cmux_N1);
                    flag_found = 0;
                    dropped_count++;
                    /* throw whole frame away*/
                    state = State::SearchForNewFrame;
                    lengthNeeded = 5;
                    fcs = 0xFF;
                    goto search_for_new_frame;
                }


                state = State::ParseFramePayload;

                if (!(datacount >= lengthNeeded)) {
                    LOG_DEBUG(
                            "Leave, frame extraction cancelled. Frame not completely stored in re-assembly buffer yet");
                    Inc();
                    return nullptr;
                }

                Inc();

            }
            else{
                LOG_DEBUG("Leave, not enough bytes stored in buffer for header information yet");
                return nullptr;
            }
        }

        case State ::ParseFramePayload:
        {
            /*Okay, done with the frame header. Start extracting the payload data */
            if (currentFrame->length > 0)
            {
                {
                    end = endp - readp;
                    if (currentFrame->length > end) /*wrap-around necessary*/
                    {
                        currentFrame->data = readp;
                        readp = data + (currentFrame->length - end);
                        datacount -= currentFrame->length;
                    }
                    else
                    {
                        currentFrame->data = readp;
                        readp += currentFrame->length;
                        datacount -= currentFrame->length;
                        if (readp == endp)
                            readp = data;
                    }
                    if (GSM0710_FRAME_IS(MuxDefines ::GSM0710_TYPE_UI, currentFrame.get()))
                    {
                        for (end = 0; end < currentFrame->length; end++)
                            fcs = crcTable[fcs ^ (currentFrame->data[end])];
                    }
                }
            }

            /*Okay, check FCS*/
            if (crcTable[fcs ^ (*readp)] != 0xCF)
            {
                Inc();
                if (*readp != static_cast<unsigned char>(MuxDefines ::GSM0710_FRAME_FLAG)) /* the FCS didn't match, but the next byte may not even be an end-frame-flag*/
                {
                    LOG_WARN("Dropping frame: Corrupt! End flag not present and FCS mismatch.");
                    flag_found = 0;
                    dropped_count++;
                    /* throw whole frame away*/
                    state = State ::SearchForNewFrame;
                    lengthNeeded = 5;
                    fcs = 0xFF;
                    goto search_for_new_frame;
                }
                else
                {
                    LOG_WARN("Dropping frame: FCS doesn't match");
                    flag_found = 0;
                    dropped_count++;
                    /* throw whole frame away*/
                    state = State ::SearchForNewFrame;
                    lengthNeeded = 5;
                    fcs = 0xFF;
                    goto search_for_new_frame;
                }
            }
            else
            {
                /*Okay, check end flag */
                Inc();
                if (*readp != static_cast<unsigned char>(MuxDefines ::GSM0710_FRAME_FLAG))
                {
                    LOG_WARN("Dropping frame: End flag not present. Instead: %d", *readp);
                    flag_found = 0;
                    dropped_count++;
                    /* throw whole frame away*/
                    state = State ::SearchForNewFrame;
                    lengthNeeded = 5;
                    fcs = 0xFF;
                    goto search_for_new_frame;
                }
                else {
                    received_count++;
                }

                Inc(); /* prepare readp for next frame extraction */

                /* Everything went fine*/
                flag_found = 0; /* prepare for any future frame processing*/
                fcs = 0xFF;
                lengthNeeded = 5;
                state = State ::SearchForNewFrame;
                LOG_DEBUG("Leave, frame found");
                memcpy(frame,currentFrame.get(),sizeof(GSM0710Frame));
                return currentFrame.get();
            }
        }

    }

    search_for_new_frame:
    return GetCompleteFrame(frame);

}

unsigned char GSM0710Buffer::frameCalcCRC(const unsigned char *input, int length) {
    unsigned char fcs = 0xFF;
    int i;
    for (i = 0; i < length; i++)
        fcs = crcTable[fcs ^ input[i]];
    return 0xFF - fcs;
}

void GSM0710Buffer::ReorganizeBuffer() {
    if (readp !=
        data) { //relayout data in cache_buf
        if (GetDataLength()) {
            LOG_DEBUG("memmove(0, %ld, %d)", (long)(readp - data), GetDataLength());
            memmove(data, readp,
                    GetDataLength());
        }
        readp = data;
        writep =
                data + GetDataLength();
    }
}