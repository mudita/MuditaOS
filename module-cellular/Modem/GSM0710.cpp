
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


std::unique_ptr<GSM0710Frame> GSM0710Buffer::GetCompleteFrame(std::unique_ptr<GSM0710Frame> frame) {
    int end;
    unsigned int length_needed = 5;// channel, type, length, fcs, flag
    unsigned char fcs = 0xFF;
    unsigned char *local_readp;
    unsigned int local_datacount, local_datacount_backup;

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

    /*skip empty frames (this causes troubles if we're using DLC 62) - skipping frame start flags*/
    while (GetDataLength() > 0 && (*readp == static_cast<unsigned char>(MuxDefines ::GSM0710_FRAME_FLAG)))
    {
        Inc();
    }

    /* Okay, we're ready to analyze a proper frame header */

    /*Make local copy of buffer pointer and data counter. They are shared between 2 threads, so we want to update them only after a frame extraction */
    /*From now on, only operate on these local copies */
    local_readp = readp;
    local_datacount = local_datacount_backup = datacount; /* current no. of stored bytes in buffer */

    if (local_datacount >= length_needed) /* enough data stored for 0710 frame header+footer? */
    {
        if (frame != NULL)
        {
            frame->channel = ((*local_readp & 252) >> 2); /*frame header address-byte read*/
            if (frame->channel > vir_ports ) /* Field Sanity check if channel ID actually exists */
            {
                LOG_WARN("Dropping frame: Corrupt! Channel Addr. field indicated %d, which does not exist",frame->channel);
                flag_found = 0;
                dropped_count++;
                goto update_buffer_dropping_frame; /* throw whole frame away, up until and incl. local_readp */
            }
            fcs = crcTable[fcs ^ *local_readp];
            Inc();
            length_needed--;
            frame->control = *local_readp; /*frame header type-byte read*/
            fcs = crcTable[fcs ^ *local_readp];
            Inc();
            length_needed--;
            frame->length = (*local_readp & 254) >> 1; /*Frame header 1st length-byte read*/
            fcs = crcTable[fcs ^ *local_readp];
        }

        if ((*local_readp & 1) == 0)/*if frame payload length byte extension bit not set, a 2nd length byte is in header*/
        {
            //Current spec (version 7.1.0) states these kind of
            //frames to be invalid Long lost of sync might be
            //caused if we would expect a long frame because of an
            //error in length field.
            Inc();
            frame->length += (*local_readp*128); /*Frame header 2nd length-byte read*/
            fcs = crcTable[fcs^*local_readp];
        }

        length_needed += frame->length; /*length_needed : 1 length byte + payload + 1 fcs byte + 1 end frame flag */
        LOG_DEBUG("length_needed: %d, available in local_datacount: %d",length_needed,local_datacount);

        if (frame->length > cmux_N1) /* Field Sanity check if payload is bigger than the max size negotiated in +CMUX */
        {
            LOG_WARN("Dropping frame: Corrupt! Length field indicated %d. Max %d allowed",frame->length, cmux_N1);
            flag_found = 0;
            dropped_count++;
            goto update_buffer_dropping_frame; /* throw whole frame away, up until and incl. local_readp */
        }
        if (!(local_datacount >= length_needed))
        {
            LOG_DEBUG("Leave, frame extraction cancelled. Frame not completely stored in re-assembly buffer yet");
            return NULL;
        }
        Inc();

/*Okay, done with the frame header. Start extracting the payload data */
        if (frame->length > 0)
        {
            {
                end = endp - local_readp;
                if (frame->length > end) /*wrap-around necessary*/
                {
                    frame->data = local_readp;
                    local_readp = data + (frame->length - end);
                    local_datacount -= frame->length;
                }
                else
                {
                    frame->data = local_readp;
                    local_readp += frame->length;
                    local_datacount -= frame->length;
                    if (local_readp == endp)
                        local_readp = data;
                }
                if (GSM0710_FRAME_IS(MuxDefines ::GSM0710_TYPE_UI, frame.get()))
                {
                    for (end = 0; end < frame->length; end++)
                        fcs = crcTable[fcs ^ (frame->data[end])];
                }
            }
        }
        /*Okay, check FCS*/
        if (crcTable[fcs ^ (*local_readp)] != 0xCF)
        {
            Inc();
            if (*local_readp != static_cast<unsigned char>(MuxDefines ::GSM0710_FRAME_FLAG)) /* the FCS didn't match, but the next byte may not even be an end-frame-flag*/
            {
                LOG_WARN("Dropping frame: Corrupt! End flag not present and FCS mismatch.");
                flag_found = 0;
                dropped_count++;
                goto update_buffer_dropping_frame; /* throw whole frame away, up until and incl. local_readp */
            }
            else
            {
                LOG_WARN("Dropping frame: FCS doesn't match");
                flag_found = 0;
                dropped_count++;
                goto update_buffer_dropping_frame; /* throw whole frame away, up until and incl. local_readp */
            }
        }
        else
        {
        /*Okay, check end flag */
            Inc();
            if (*local_readp != static_cast<unsigned char>(MuxDefines ::GSM0710_FRAME_FLAG))
            {
                LOG_WARN("Dropping frame: End flag not present. Instead: %d", *local_readp);
                flag_found = 0;
                dropped_count++;
                goto update_buffer_dropping_frame;
            }
            else {
                received_count++;
            }

            Inc(); /* prepare readp for next frame extraction */
        }
    }
    else
    {
        LOG_DEBUG("Leave, not enough bytes stored in buffer for header information yet");
        return nullptr;
    }

/* Everything went fine*/

    flag_found = 0; /* prepare for any future frame processing*/
    LOG_DEBUG("Leave, frame found");
    return frame;

    update_buffer_dropping_frame:
    return GetCompleteFrame(std::move(frame));    /*continue extracting more frames if any*/
}

unsigned char GSM0710Buffer::frameCalcCRC(const unsigned char *input, int length) {
    unsigned char fcs = 0xFF;
    int i;
    for (i = 0; i < length; i++)
        fcs = crcTable[fcs ^ input[i]];
    return 0xFF - fcs;
}