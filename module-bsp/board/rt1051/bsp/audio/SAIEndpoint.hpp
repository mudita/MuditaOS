#pragma once

#include <Audio/Stream.hpp>
#include <Audio/Endpoint.hpp>

#include "fsl_sai_edma.h"

namespace bsp
{
    class SAIEndpoint
    {
      protected:
        void start();
        void stop();

        sai_edma_handle_t *_handle = nullptr;
        I2S_Type *_base;

      public:
        SAIEndpoint() = delete;
        SAIEndpoint(sai_edma_handle_t *handle, I2S_Type *base);

        virtual void initiateSAITransfer() = 0;
    };

    class SAISource : public SAIEndpoint, public audio::Source
    {
      public:
        SAISource() = delete;
        SAISource(sai_edma_handle_t *handle, I2S_Type *base);
        void dataWrite() override;
        void initiateSAITransfer() override;
        void start() override;
        void stop() override;
    };

    class SAISink : public SAIEndpoint, public audio::Sink
    {
      public:
        SAISink() = delete;
        SAISink(sai_edma_handle_t *handle, I2S_Type *base);
        void dataRead() override;
        void initiateSAITransfer() override;
        void start() override;
        void stop() override;
    };

}; // namespace bsp
