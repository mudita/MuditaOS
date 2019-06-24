
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
                //TODO:M.P implement
                if (worker->ExtractFrames()) {
                    int vir_port = 1;
/*                    for (vir_port = 1; vir_port <= vir_ports; vir_port++) {
                        pseudo_device_write(&channellist[vir_port], NULL, 0); //flush all data to virtual port
                    }*/
                }

                if (worker->muxDaemon->inputBuffer->readp !=
                    worker->muxDaemon->inputBuffer->data) { //relayout data in cache_buf
                    if (worker->muxDaemon->inputBuffer->GetDataLength()) {
                        //LOGMUX(LOG_DEBUG, "memmove(0, %ld, %d)", (long)(serial_p->in_buf->readp - serial_p->in_buf->data), gsm0710_buffer_length(serial_p->in_buf));
                        memmove(worker->muxDaemon->inputBuffer->data, worker->muxDaemon->inputBuffer->readp,
                                worker->muxDaemon->inputBuffer->GetDataLength());
                    }
                    worker->muxDaemon->inputBuffer->readp = worker->muxDaemon->inputBuffer->data;
                    worker->muxDaemon->inputBuffer->writep =
                            worker->muxDaemon->inputBuffer->data + worker->muxDaemon->inputBuffer->GetDataLength();
                }
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
                LOG_DEBUG("Free space is not enough");
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
    std::unique_ptr<GSM0710Frame> frame;

    while (muxDaemon->inputBuffer->GetCompleteFrame(std::move(frame)) != NULL) {
        frames_extracted++;

        if ((muxDaemon->inputBuffer->GSM0710_FRAME_IS(MuxDefines::GSM0710_TYPE_UI, frame.get()) ||
             muxDaemon->inputBuffer->GSM0710_FRAME_IS(MuxDefines::GSM0710_TYPE_UIH, frame.get()))) {
            LOG_DEBUG("Frame is UI or UIH");
            if (frame->control & static_cast<unsigned char>(MuxDefines::GSM0710_PF)) {
                muxDaemon->uih_pf_bit_received = 1;
            }

            //data from logical channel
            if (frame->channel > 0) {
                LOG_DEBUG("Writing %d byte frame received on channel %d to %s", frame->length, frame->channel,
                          muxDaemon->channels[frame->channel].GetName().c_str());

                int write_result;

                if (muxDaemon->channels[frame->channel].GetState() ==
                    MuxChannel::State::Opened) {//reopening, discard the data
                    // TODO:M.P distribute frame to virtual channel
                    // write_result = pseudo_device_write(&channellist[frame->channel], frame->data, frame->length);
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
                // TODO:M.P implement control channel command handling
                //handle_command(frame);
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
                        //TODO:M.P implement logical channel close
                        // SYSCHECK(logical_channel_close(channellist+frame->channel));
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
                    if (frame->channel == muxDaemon->virtualPortsCount)
                        //TODO: M.P dunno what is this for ??
                        // ql_cmux_debug = 0;
                        break;
                case MuxDefines::GSM0710_TYPE_DM:
                    if (muxDaemon->channels[frame->channel].GetState() == MuxChannel::State::Opened) {
                        //TODO:M.P implement logical channel close
                        // SYSCHECK(logical_channel_close(channellist+frame->channel));
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
                        //TODO:M.P implement write frame
                        // write_frame(frame->channel, NULL, 0, GSM0710_TYPE_UA | GSM0710_PF);
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
                        //TODO:M.P implement write frame
                        //write_frame(frame->channel, NULL, 0, GSM0710_TYPE_DM | GSM0710_PF);
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
                    //TODO:M.P implement write frame
                    // write_frame(frame->channel, NULL, 0, GSM0710_TYPE_UA | GSM0710_PF);
                    if (frame->channel == muxDaemon->virtualPortsCount)
                        //TODO: M.P dunno what is this for ??
                        // ql_cmux_debug = 0;
                        break;
            }
        }
    }

    return frames_extracted;

}