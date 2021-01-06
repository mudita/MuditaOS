#include "SAIAudioDevice.hpp"

using namespace bsp;

SAIAudioDevice::SAIAudioDevice(AudioDevice::audioCallback_t callback,
                               I2S_Type *base,
                               sai_edma_handle_t *rxHandle,
                               sai_edma_handle_t *txHandle)
    : AudioDevice(callback), rx(rxHandle), tx(txHandle)
{}

void initiateRxTransfer()
{}

void initiateTxTransfer()
{}
