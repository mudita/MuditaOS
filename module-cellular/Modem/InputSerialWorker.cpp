
/*
 * @file InputSerialWorker.cpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 24.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#include "InputSerialWorker.hpp"
#include "MuxDaemon.hpp"
#include "log/log.hpp"


InputSerialWorker::InputSerialWorker(MuxDaemon *mux) : muxDaemon(mux) {

}


void workerTaskFunction(void *ptr) {
    InputSerialWorker *worker = reinterpret_cast<InputSerialWorker *>(ptr);

    while (1) {
        worker->muxDaemon->cellular->Wait(UINT32_MAX);
        if (worker->ReadIncomingData() > 0) {
            if (worker->muxDaemon->inputBuffer->GetDataLength() > 0) {

                if (worker->ExtractFrames()) {
                    //TODO:M.P implement error handling ?
                }

/*                //TODO:M.P sth broken here
                if (worker->muxDaemon->inputBuffer->readp !=
                    worker->muxDaemon->inputBuffer->data) { //relayout data in cache_buf
                    if (worker->muxDaemon->inputBuffer->GetDataLength()) {
                        LOG_DEBUG("memmove(0, %ld, %d)", (long)(worker->muxDaemon->inputBuffer->readp - worker->muxDaemon->inputBuffer->data), worker->muxDaemon->inputBuffer->GetDataLength());
                        memmove(worker->muxDaemon->inputBuffer->data, worker->muxDaemon->inputBuffer->readp,
                                worker->muxDaemon->inputBuffer->GetDataLength());
                    }
                    worker->muxDaemon->inputBuffer->readp = worker->muxDaemon->inputBuffer->data;
                    worker->muxDaemon->inputBuffer->writep =
                            worker->muxDaemon->inputBuffer->data + worker->muxDaemon->inputBuffer->GetDataLength();
                }*/
            }
        }
    }
}

bool InputSerialWorker::Init() {
    BaseType_t task_error = xTaskCreate(
            workerTaskFunction,
            "cellInSerWorker",
            512, // in words
            this,
            taskPriority,
            &taskHandle);
    if (task_error != pdPASS) {
        LOG_ERROR("Failed to start inputSerialWorker task");
        return false;
    }
    return true;
}

bool InputSerialWorker::Deinit() {
    if (taskHandle) {
        vTaskDelete(taskHandle);
    }
}

int InputSerialWorker::ReadIncomingData() {
    int length = 0;

    switch (muxDaemon->state) {
        case MuxDaemon::States::MUX_STATE_MUXING:
            length = std::min(muxDaemon->inputBuffer->GetFreeSpace(),
                              (uint32_t) (muxDaemon->inputBuffer->endp - muxDaemon->inputBuffer->writep));

            if (length > 0) /*available space in buffer (not locked since we want to utilize all available space)*/
            {
                length = muxDaemon->cellular->Read(muxDaemon->inputBuffer->writep, length);
                if (length > 0) {
                    LOG_DEBUG("READ SIZE : %d", length);
                    //syslogdump("<s ", serial->in_buf->writep, length);
                    muxDaemon->inputBuffer->writep += length;
                    if (muxDaemon->inputBuffer->writep == muxDaemon->inputBuffer->endp)
                        muxDaemon->inputBuffer->writep = muxDaemon->inputBuffer->data;

                    muxDaemon->inputBuffer->datacount += length; /*updating the data-not-yet-read counter*/
                    LOG_DEBUG("GSM0710 buffer (up-to-date): written %d, free %d, stored %d",
                              length, muxDaemon->inputBuffer->GetFreeSpace(), muxDaemon->inputBuffer->GetDataLength());

                    if (length > muxDaemon->inputBuffer->max_count) {
                        LOG_INFO("MAX READ SIZE : %d", length);
                        LOG_INFO("GSM0710 buffer (up-to-date): written %d, free %d, stored %d",
                                 length, muxDaemon->inputBuffer->GetFreeSpace(),
                                 muxDaemon->inputBuffer->GetDataLength());
                        muxDaemon->inputBuffer->max_count = length;
                    }
                } else {
                    LOG_INFO("READ SIZE : %d", length);
                }
            } else {
                LOG_DEBUG("Not enough space in buffer");
            }
            break;

        default:
            LOG_WARN("Don't know how to handle reading in state %d", muxDaemon->state);
            break;
    }

    return length;
}

int InputSerialWorker::ExtractFrames() {
    int frames_extracted = 0;
    GSM0710Frame local_frame;
    GSM0710Frame *frame = &local_frame;

    while (muxDaemon->inputBuffer->GetCompleteFrame(frame) != NULL) {
        frames_extracted++;

        if ((muxDaemon->inputBuffer->GSM0710_FRAME_IS(MuxDefines::GSM0710_TYPE_UI, frame) ||
             muxDaemon->inputBuffer->GSM0710_FRAME_IS(MuxDefines::GSM0710_TYPE_UIH, frame))) {
            LOG_DEBUG("Frame is UI or UIH");
            if (frame->control & static_cast<unsigned char>(MuxDefines::GSM0710_PF)) {
                muxDaemon->uih_pf_bit_received = 1;
            }

            //data from logical channel
            if (frame->channel > 0) {
                LOG_DEBUG("Writing %d byte frame received on channel %d to %s", frame->length, frame->channel,
                          muxDaemon->channels[frame->channel].GetName().c_str());

                int write_result=0;

                if (muxDaemon->channels[frame->channel].GetState() ==
                    MuxChannel::State::Opened) {//reopening, discard the data

                    // Send received message to virtual channel for further processing
                    write_result = muxDaemon->channels[frame->channel].Send(frame->data,frame->length);
                } else {
                    LOG_INFO("channel %d closed, discard the frame", frame->channel);
                    write_result = frame->length;
                }

                if (write_result < frame->length) {
                    LOG_INFO("write() returned. Written %d/%d bytes of frame to %s",
                             write_result, frame->length, muxDaemon->channels[frame->channel].GetName().c_str());

                    //send_modem_status(frame->channel,1);
                    //fill_in_list(frame->channel,frame->data + write_result,frame->length - write_result);
                    //MSC_flag |=  1 << (frame->channel - 1);
                }
            } else {
                //control channel command
                LOG_DEBUG("Frame channel == 0, control channel command");
                HandleCtrlChannelCommands(frame);
            }
        } else {
            //not an information frame
            LOG_DEBUG("Not an information frame");
            MuxDefines frameType = static_cast<MuxDefines >(frame->control &
                                                            ~static_cast<unsigned char>(MuxDefines::GSM0710_PF));
            switch (frameType) {
                case MuxDefines::GSM0710_TYPE_UA:
                    LOG_DEBUG("Frame is UA");
                    if (muxDaemon->channels[frame->channel].GetState() == MuxChannel::State::Opened) {
                        // Remove channel
                        muxDaemon->channels.erase(muxDaemon->channels.begin() + frame->channel);
                        LOG_INFO("Logical channel %d for %s closed",
                                 frame->channel, muxDaemon->channels[frame->channel].GetName().c_str());
                    } else {
                        if (muxDaemon->channels[frame->channel].disc_ua_pending == 0) {
                            muxDaemon->channels[frame->channel].SetState(MuxChannel::State::Opened);
                            if (frame->channel == 0) {
                                LOG_DEBUG("Control channel opened");
                            } else
                                LOG_INFO("Logical channel %d opened", frame->channel);
                        } else {
                            LOG_INFO("UA to acknowledgde DISC on channel %d received", frame->channel);
                            muxDaemon->channels[frame->channel].disc_ua_pending = 0;
                        }
                    }
                    //TODO: M.P dunno what is this for ??
#if 0
                    if (frame->channel == muxDaemon->virtualPortsCount){
                        ql_cmux_debug = 0;
                        }
#endif
                        break;
                case MuxDefines::GSM0710_TYPE_DM:
                    if (muxDaemon->channels[frame->channel].GetState() == MuxChannel::State::Opened) {
                        // Remove channel
                        muxDaemon->channels.erase(muxDaemon->channels.begin() + frame->channel);
                        LOG_INFO("DM received, so the channel %d for %s was already closed",
                                 frame->channel, muxDaemon->channels[frame->channel].GetName().c_str());
                    } else {
                        if (frame->channel == 0) {
                            LOG_INFO("Couldn't open control channel.\n->Terminating");
                            muxDaemon->state = MuxDaemon::States::MUX_STATE_CLOSING;
//close channels
                        } else
                            LOG_INFO("Logical channel %d for %s couldn't be opened", frame->channel,
                                     muxDaemon->channels[frame->channel].GetName().c_str());
                    }
                    break;
                case MuxDefines::GSM0710_TYPE_DISC:
                    if (muxDaemon->channels[frame->channel].GetState() == MuxChannel::State::Opened) {
                        muxDaemon->channels[frame->channel].SetState(MuxChannel::State::Closed);

                        muxDaemon->WriteMuxFrame(frame->channel, NULL, 0,
                                                 static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_UA) |
                                                 static_cast<unsigned char>(MuxDefines::GSM0710_PF));

                        if (frame->channel == 0) {
                            muxDaemon->state = MuxDaemon::States::MUX_STATE_CLOSING;
                            LOG_INFO("Control channel closed");
                        } else
                            LOG_INFO("Logical channel %d for %s closed", frame->channel,
                                     muxDaemon->channels[frame->channel].GetName().c_str());
                    } else {
//channel already closed
                        LOG_WARN("Received DISC even though channel %d for %s was already closed",
                                 frame->channel, muxDaemon->channels[frame->channel].GetName().c_str());
                        // Send Mux frame
                        muxDaemon->WriteMuxFrame(frame->channel, NULL, 0,
                                                 static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_DM) |
                                                 static_cast<unsigned char>(MuxDefines::GSM0710_PF));
                    }
                    break;
                case MuxDefines::GSM0710_TYPE_SABM:
//channel open request
                    if (muxDaemon->channels[frame->channel].GetState() == MuxChannel::State::Opened) {
                        if (frame->channel == 0)
                            LOG_INFO("Control channel opened");
                        else
                            LOG_INFO("Logical channel %d for %s opened",
                                     frame->channel, muxDaemon->channels[frame->channel].GetName().c_str());
                    } else
//channel already opened
                        LOG_WARN("Received SABM even though channel %d for %s was already closed",
                                 frame->channel, muxDaemon->channels[frame->channel].GetName().c_str());
                    muxDaemon->channels[frame->channel].SetState(MuxChannel::State::Opened);
                    // Send mux frame
                    muxDaemon->WriteMuxFrame(frame->channel, NULL, 0,
                                             static_cast<unsigned char>(MuxDefines::GSM0710_TYPE_UA) |
                                             static_cast<unsigned char>(MuxDefines::GSM0710_PF));
//TODO: M.P dunno what is this for ??
#if 0
                    if (frame->channel == muxDaemon->virtualPortsCount){
                        ql_cmux_debug = 0;
                        }
#endif
                        break;

                default:
                    break;
            }
        }
    }

    return frames_extracted;

}

int InputSerialWorker::HandleCtrlChannelCommands(GSM0710Frame* frame) {
    unsigned char type, signals;
    int length = 0, i, type_length, channel, supported = 1;
    unsigned char *response;

    //struct ussp_operation op;
    if (frame->length > 0)
    {
        type = frame->data[0];// only a byte long types are handled now skip extra bytes
        for (i = 0; (frame->length > i && (frame->data[i] & static_cast<unsigned char>(MuxDefines ::GSM0710_EA)) == 0); i++);
        i++;
        type_length = i;
        if ((type & static_cast<unsigned char>(MuxDefines ::GSM0710_CR)) == static_cast<unsigned char>(MuxDefines ::GSM0710_CR))
        {
//command not ack extract frame length
            while (frame->length > i)
            {
                length = (length * 128) + ((frame->data[i] & 254) >> 1);
                if ((frame->data[i] & 1) == 1)
                    break;
                i++;
            }
            i++;
            MuxDefines ftype = static_cast<MuxDefines >(type & ~static_cast<unsigned char>(MuxDefines ::GSM0710_CR));
            switch (ftype)
            {
                case MuxDefines ::GSM0710_CONTROL_CLD:
                    //TODO: M.P implement CMUX close
                    LOG_INFO("The mobile station requested mux-mode termination");
                    muxDaemon->state = MuxDaemon::States ::MUX_STATE_CLOSING;
                    break;
                case MuxDefines ::GSM0710_CONTROL_PSC:
                    LOG_INFO("Power Service Control command: ***");
                    LOG_INFO("Frame->data = %s / frame->length = %d", frame->data + i, frame->length - i);
                    break;
                case MuxDefines ::GSM0710_CONTROL_TEST:
                    LOG_INFO("Test command: ");
                    LOG_INFO("Frame->data = %s / frame->length = %d", frame->data + i, frame->length - i);
                    //serial->ping_number = 0;
                    break;
                case MuxDefines ::GSM0710_CONTROL_MSC:
                    LOG_INFO("GET MSC");
                    if (i + 1 < frame->length)
                    {
                        channel = ((frame->data[i] & 252) >> 2);
                        i++;
                        signals = (frame->data[i]);
//op.op = USSP_MSC;
//op.arg = USSP_RTS;
//op.len = 0;
                        LOG_INFO("Modem status command on channel %d", channel);
                        muxDaemon->channels[channel].frameAllowed = ((signals &
                                static_cast<unsigned char>(MuxDefines ::GSM0710_SIGNAL_FC)) != static_cast<unsigned char>(MuxDefines ::GSM0710_SIGNAL_FC));
                        if ((signals & static_cast<unsigned char>(MuxDefines ::GSM0710_SIGNAL_FC)) == static_cast<unsigned char>(MuxDefines ::GSM0710_SIGNAL_FC))
                            LOG_INFO("No frames allowed");
                        else
                        {
//op.arg |= USSP_CTS;
                            LOG_INFO("Frames allowed");
                        }
                        if ((signals & static_cast<unsigned char>(MuxDefines ::GSM0710_SIGNAL_RTC)) == static_cast<unsigned char>(MuxDefines ::GSM0710_SIGNAL_RTC))
                        {
//op.arg |= USSP_DSR;
                            LOG_INFO("Signal RTC");
                        }
                        if ((signals & static_cast<unsigned char>(MuxDefines ::GSM0710_SIGNAL_IC)) == static_cast<unsigned char>(MuxDefines ::GSM0710_SIGNAL_IC))
                        {
//op.arg |= USSP_RI;
                            LOG_INFO("Signal Ring");
                        }
                        if ((signals & static_cast<unsigned char>(MuxDefines ::GSM0710_SIGNAL_DV)) == static_cast<unsigned char>(MuxDefines ::GSM0710_SIGNAL_DV))
                        {
//op.arg |= USSP_DCD;
                            LOG_INFO("Signal DV");
                        }
                    }
                    else
                        LOG_ERROR("Modem status command, but no info. i: %d, len: %d, data-len: %d",
                               i, length, frame->length);
                    break;
                default:
                    LOG_ERROR("Unknown command (%d) from the control channel", type);
                    if ((response = static_cast<unsigned char*>(malloc(sizeof(char) * (2 + type_length)))) != NULL)
                    {
                        i = 0;
                        response[i++] = static_cast<unsigned char>(MuxDefines ::GSM0710_CONTROL_NSC);
                        type_length &= 127; //supposes that type length is less than 128
                        response[i++] = static_cast<unsigned char>(MuxDefines ::GSM0710_EA) | (type_length << 1);
                        while (type_length--)
                        {
                            response[i] = frame->data[i - 2];
                            i++;
                        }
                        muxDaemon->WriteMuxFrame(0, response, i, static_cast<unsigned char>(MuxDefines ::GSM0710_TYPE_UIH));
                        free(response);
                        supported = 0;
                    }
                    else
                        LOG_ERROR("Out of memory, when allocating space for response");
                    break;
            }
            if (supported)
            {
//acknowledge the command
                frame->data[0] = frame->data[0] & ~static_cast<unsigned char>(MuxDefines ::GSM0710_CR);
                LOG_INFO("response MSC...");
                muxDaemon->WriteMuxFrame(0, frame->data, frame->length, static_cast<unsigned char>(MuxDefines ::GSM0710_TYPE_UIH));
                LOG_INFO("response MSC");
#if 0
                switch ((type & ~GSM0710_CR)){
                  case GSM0710_CONTROL_MSC:
                    if (frame->control & GSM0710_PF){ //Check if the P/F var needs to be set again (cleared in write_frame)
                      uih_pf_bit_received = 1;
                    }
                    LOGMUX(LOG_DEBUG, "Sending 1st MSC command App->Modem");
                    frame->data[0] = frame->data[0] | GSM0710_CR; //setting the C/R bit to "command"
                    write_frame(0, frame->data, frame->length, GSM0710_TYPE_UIH);
                    break;
                  default:
                    break;
                }
#endif
            }
        }
        else
        {
//received ack for a command
            if (GSM0710Buffer::GSM0710_COMMAND_IS(static_cast<MuxDefines >(type), static_cast<unsigned char>(MuxDefines ::GSM0710_CONTROL_NSC)))
                LOG_ERROR("The mobile station didn't support the command sent");
            else if(GSM0710Buffer::GSM0710_COMMAND_IS(static_cast<MuxDefines >(type),static_cast<unsigned char>(MuxDefines ::GSM0710_CONTROL_MSC)))
            {
                LOG_INFO("Channel:%d FC:%d",(frame->data[i] & 252) >> 2,frame->data[0]&0x2);
                LOG_INFO("\n\nGET ACK FOR MSC\n\n");
                //sleep(1);
            }
            else
                LOG_INFO("Command acknowledged by the mobile station");
        }
    }
    return 0;
}