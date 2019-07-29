/*
 *  @file rt1051_audiocodec.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_RT1051_AUDIOCODEC_HPP
#define PUREPHONE_RT1051_AUDIOCODEC_HPP


#include "audio/bsp_audio.hpp"
#include "fsl_sai_edma.h"

namespace bsp{

    class RT1051Audiocodec : public AudioDevice{
    public:
        RT1051Audiocodec(AudioDevice::audioCallback_t callback);
        virtual ~RT1051Audiocodec();

        int32_t Start(const Format& format) override final;
        int32_t Stop() override final;
        int32_t OutputVolumeCtrl(float vol) override final;
        int32_t InputGainCtrl(float gain) override final;
        int32_t OutputPathCtrl(uint32_t outputPath) override final;
        int32_t InputPathCtrl(uint32_t inputPath) override final;
        bool IsFormatSupported(const Format& format) override final;

    private:

        /*! @brief Internals state of Rx/Tx callback, needed for double buffering technique */
        enum class irq_state_t
        {
            IRQStateHalfTransfer=0,
            IRQStateFullTransfer=1
        };

        sai_config_t config;

        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t txHandle);
        static edma_handle_t dmaTxHandle;
        static AT_NONCACHEABLE_SECTION_INIT(sai_edma_handle_t rxHandle);
        static edma_handle_t dmaRxHandle;

        void Init();
        void Deinit();

        bool TryOpenStream(const bsp::AudioDevice::Format &format);


        static void txCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);
        static void rxCallback(I2S_Type *base, sai_edma_handle_t *handle, status_t status, void *userData);

    };
}


#endif //PUREPHONE_RT1051_AUDIOCODEC_HPP
