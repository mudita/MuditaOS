// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "BspEink.hpp"

#include "board.h"
#include "fsl_lpspi.h"
#include "fsl_lpspi_edma.h"

#include "dma_config.h"
#include "bsp/eink/eink_gpio.hpp"

#include "FreeRTOS.h"
#include "semphr.h"

#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"
#include "drivers/gpio/DriverGPIO.hpp"
#include "board/BoardDefinitions.hpp"

namespace bsp::eink
{
    using namespace drivers;

    namespace
    {
        enum class BusyWaitEventState
        {
            Registered,
            NotRegistered
        };

        struct LpspiResource
        {
            LPSPI_Type *base;
            std::uint32_t instance;
            std::uint32_t (*GetFreq)();
        };

        struct LpspiEdmaResource
        {
            DMA_Type *txEdmaBase;
            std::uint32_t txEdmaChannel;
            std::uint8_t txDmaRequest;

            DMA_Type *rxEdmaBase;
            std::uint32_t rxEdmaChannel;
            std::uint8_t rxDmaRequest;

#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
            DMAMUX_Type *txDmamuxBase;
            DMAMUX_Type *rxDmamuxBase;
#endif
        };

        struct BspEinkDriver
        {
            LpspiResource *resource;
            LpspiEdmaResource *dmaResource;
            lpspi_master_edma_handle_t *handle;
            edma_handle_t *edmaRxRegToRxDataHandle;
            edma_handle_t *edmaTxDataToTxRegHandle;
            std::uint32_t baudRate_Bps;
            std::uint8_t flags; // Control and state flags

            BusyEventCallback event;
            SPICSConfig csConfig;             // Current transfer chip select configuration (automatic of manual)
            BusyWaitEventState eventRegister; // Tells if something is waiting for not busy event
        };

        lpspi_master_config_t spi_config;
        std::shared_ptr<DriverGPIO> gpio;
        std::shared_ptr<DriverDMA> dma;
        std::shared_ptr<DriverDMAMux> dmamux;
        std::unique_ptr<DriverDMAHandle> rxDMAHandle;
        std::unique_ptr<DriverDMAHandle> txDMAHandle;

        constexpr auto transferWriteClockHz = 18'000'000;
        constexpr auto transferReadClockHz  = 3'000'000;
        constexpr auto delayInNanoSec       = 1'000'000'000 / transferWriteClockHz;
        const auto lpspiBase                = BOARD_EINK_LPSPI_BASE;

        std::uint32_t lpspiGetFreq()
        {
            return GetPerphSourceClock(PerphClock_LPSPI);
        }

        LpspiResource lpspiResource = {lpspiBase, 1, lpspiGetFreq};

        LpspiEdmaResource lpspiEdmaResource = {
            BSP_EINK_LPSPI_DMA_TX_DMA_BASE,
            BSP_EINK_LPSPI_DMA_TX_CH,
            BSP_EINK_LPSPI_DMA_TX_PERI_SEL,
            BSP_EINK_LPSPI_DMA_RX_DMA_BASE,
            BSP_EINK_LPSPI_DMA_RX_CH,
            BSP_EINK_LPSPI_DMA_RX_PERI_SEL,
            BSP_EINK_LPSPI_DMA_TX_DMAMUX_BASE,
            BSP_EINK_LPSPI_DMA_RX_DMAMUX_BASE,
        };

        AT_NONCACHEABLE_SECTION(lpspi_master_edma_handle_t lpspiEdmaHandle);

        bool isInitialized = false;
        QueueHandle_t transferCompleteQueue;
        SemaphoreHandle_t busySemaphore; // Suspends the task until the EPD display is busy

        BspEinkDriver lpspiEdmaDriverState = {
            &lpspiResource,
            &lpspiEdmaResource,
            &lpspiEdmaHandle,
            nullptr, // Will be filled in init function
            nullptr, // Will be filled in init function
            0,
            0,
            nullptr,
            SPICSConfig::Automatic,
            BusyWaitEventState::Registered,
        };

        void lpspiMasterEdmaCallback(LPSPI_Type *base,
                                     lpspi_master_edma_handle_t *handle,
                                     status_t status,
                                     void *userData)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;

            if (xQueueSendFromISR(transferCompleteQueue, &status, &xHigherPriorityTaskWoken) != pdPASS) {
                /* If we reached this point, something very bad happened */
                return;
            }

            /* Disable SPI for current consumption reduction */
            LPSPI_Enable(lpspiBase, false);

            portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
        }
    } // namespace

    status_t init()
    {
        if (isInitialized) {
            return kStatus_Success;
        }

        /* If busy semaphore already created - delete it */
        if (busySemaphore != nullptr) {
            vSemaphoreDelete(busySemaphore);
        }

        /* Create new busy semaphore */
        busySemaphore = xSemaphoreCreateBinary();
        if (busySemaphore == nullptr) {
            return kStatus_Fail;
        }

        /* At the beginning the semaphore is in unknown state, it has to be clearly taken or given */
        xSemaphoreTake(busySemaphore, 0);

        /* If transfer complete queue already created - delete it */
        if (transferCompleteQueue != nullptr) {
            vQueueDelete(transferCompleteQueue);
        }

        /* Create new queue */
        transferCompleteQueue = xQueueCreate(1, sizeof(std::uint32_t));
        if (transferCompleteQueue == nullptr) {
            vSemaphoreDelete(busySemaphore);
            busySemaphore = nullptr;
            return kStatus_Fail;
        }

        auto lpspi           = &lpspiEdmaDriverState;
        lpspi->eventRegister = BusyWaitEventState::NotRegistered;

        gpio = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::EINK_GPIO), DriverGPIOParams{});
        eink_gpio_configure();

        LPSPI_MasterGetDefaultConfig(&spi_config);
        spi_config.baudRate                      = transferWriteClockHz;
        spi_config.bitsPerFrame                  = 8;
        spi_config.cpol                          = kLPSPI_ClockPolarityActiveHigh;
        spi_config.cpha                          = kLPSPI_ClockPhaseFirstEdge;
        spi_config.direction                     = kLPSPI_MsbFirst;
        spi_config.pcsToSckDelayInNanoSec        = delayInNanoSec;
        spi_config.lastSckToPcsDelayInNanoSec    = delayInNanoSec;
        spi_config.betweenTransferDelayInNanoSec = delayInNanoSec;
        spi_config.whichPcs                      = kLPSPI_Pcs0;
        spi_config.pcsActiveHighOrLow            = kLPSPI_PcsActiveLow;
        spi_config.pinCfg                        = kLPSPI_SdiInSdoOut;
        spi_config.dataOutConfig                 = kLpspiDataOutRetained;

        LPSPI_MasterInit(lpspiBase, &spi_config, lpspiGetFreq());

        /* FSL LPSPI doesn't support configuring autopcs feature */
        lpspiBase->CFGR1 |= LPSPI_CFGR1_AUTOPCS(0);

        dmamux = DriverDMAMux::Create(static_cast<DMAMuxInstances>(BoardDefinitions::EINK_DMAMUX), {});
        dma    = DriverDMA::Create(static_cast<DMAInstances>(BoardDefinitions::EINK_DMA), {});

        txDMAHandle = dma->CreateHandle(static_cast<std::uint32_t>(BoardDefinitions::EINK_TX_DMA_CHANNEL));
        rxDMAHandle = dma->CreateHandle(static_cast<std::uint32_t>(BoardDefinitions::EINK_RX_DMA_CHANNEL));

        dmamux->Enable(static_cast<std::uint32_t>(BoardDefinitions::EINK_TX_DMA_CHANNEL),
                       BSP_EINK_LPSPI_DMA_TX_PERI_SEL); // TODO: M.P fix BSP_EINK_LPSPI_DMA_TX_PERI_SEL
        dmamux->Enable(static_cast<std::uint32_t>(BoardDefinitions::EINK_RX_DMA_CHANNEL),
                       BSP_EINK_LPSPI_DMA_RX_PERI_SEL); // TODO: M.P fix BSP_EINK_LPSPI_DMA_RX_PERI_SEL

        lpspiEdmaDriverState.edmaTxDataToTxRegHandle = reinterpret_cast<edma_handle_t *>(txDMAHandle->GetHandle());
        lpspiEdmaDriverState.edmaRxRegToRxDataHandle = reinterpret_cast<edma_handle_t *>(rxDMAHandle->GetHandle());

        LPSPI_SetMasterSlaveMode(lpspiBase, kLPSPI_Master);
        LPSPI_MasterTransferCreateHandleEDMA(lpspiBase,
                                             lpspi->handle,
                                             lpspiMasterEdmaCallback,
                                             nullptr,
                                             lpspi->edmaRxRegToRxDataHandle,
                                             lpspi->edmaTxDataToTxRegHandle);

        isInitialized = true;
        return kStatus_Success;
    }

    void deinit()
    {
        if (!isInitialized) {
            return;
        }

        LPSPI_Enable(lpspiBase, false);

        if (busySemaphore != nullptr) {
            vSemaphoreDelete(busySemaphore);
            busySemaphore = nullptr;
        }

        if (transferCompleteQueue != nullptr) {
            vQueueDelete(transferCompleteQueue);
            transferCompleteQueue = nullptr;
        }

        dmamux->Disable(static_cast<std::uint32_t>(BoardDefinitions::EINK_TX_DMA_CHANNEL));
        dmamux->Disable(static_cast<std::uint32_t>(BoardDefinitions::EINK_RX_DMA_CHANNEL));
        dmamux.reset();
        dma.reset();

        gpio->DisableInterrupt(1 << static_cast<std::uint32_t>(BoardDefinitions::EINK_BUSY_PIN));
        gpio->ClearPortInterrupts(1 << static_cast<std::uint32_t>(BoardDefinitions::EINK_BUSY_PIN));
        gpio.reset();

        isInitialized = false;
    }

    void logicPowerOn()
    {
        eink_gpio_power_on();
    }

    void logicPowerOff()
    {
        eink_gpio_power_off();
    }

    void writeCS(SPICSState csState)
    {
        gpio->WritePin(static_cast<std::uint32_t>(BoardDefinitions::EINK_CS_PIN), static_cast<std::uint8_t>(csState));
    }

    bool waitUntilDisplayBusy(std::uint32_t timeout)
    {
        bool success                       = false;
        lpspiEdmaDriverState.eventRegister = BusyWaitEventState::Registered;
        if (xSemaphoreTake(busySemaphore, timeout) != pdPASS) {
            success = false;
        }
        else {
            xSemaphoreGive(busySemaphore);
            success = true;
        }

        lpspiEdmaDriverState.eventRegister = BusyWaitEventState::NotRegistered;
        return success;
    }

    void resetDisplayController()
    {
        writeCS(SPICSState::Set);

        gpio->WritePin(static_cast<std::uint32_t>(BoardDefinitions::EINK_RESET_PIN), 0);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio->WritePin(static_cast<std::uint32_t>(BoardDefinitions::EINK_RESET_PIN), 1);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    status_t changeSpiFrequency(std::uint32_t frequencyHz)
    {
        std::uint32_t tcrPrescalerValue = 0;

        LPSPI_Enable(lpspiBase, false);
        LPSPI_MasterSetBaudRate(lpspiBase, frequencyHz, lpspiGetFreq(), &tcrPrescalerValue);

        spi_config.baudRate = frequencyHz;
        lpspiBase->TCR      = LPSPI_TCR_CPOL(spi_config.cpol) | LPSPI_TCR_CPHA(spi_config.cpha) |
                         LPSPI_TCR_LSBF(spi_config.direction) | LPSPI_TCR_FRAMESZ(spi_config.bitsPerFrame - 1U) |
                         LPSPI_TCR_PRESCALE(tcrPrescalerValue) | LPSPI_TCR_PCS(spi_config.whichPcs);

        return 0;
    }

    status_t writeData(void *txBuffer, std::uint32_t len, SPICSConfig csMode)
    {
        constexpr auto txTimeoutMs = 2000U;

        status_t tx_status = -1;
        status_t status;
        lpspi_transfer_t xfer{};

        if (csMode == SPICSConfig::Automatic) {
            writeCS(SPICSState::Clear);
        }

        /* Increase the SPI clock frequency to the SPI WRITE value */
        changeSpiFrequency(transferWriteClockHz);

        lpspiEdmaDriverState.csConfig         = csMode;
        lpspiEdmaDriverState.handle->userData = &lpspiEdmaDriverState;

        /* Clean Tx complete queue */
        xQueueReset(transferCompleteQueue);

        /* Clear BUSY Pin IRQ Flag */
        gpio->ClearPortInterrupts(1 << static_cast<std::uint32_t>(BoardDefinitions::EINK_BUSY_PIN));

        /* Enable BUSY Pin IRQ */
        gpio->EnableInterrupt(1 << static_cast<std::uint32_t>(BoardDefinitions::EINK_BUSY_PIN));

        /* Take the BUSY semaphore without timeout just in case the transmission makes the BUSY pin state change. It
         * enables the driver to block then on the busySemaphore until the BUSY pin is deasserted. */
        xSemaphoreTake(busySemaphore, 0);

        const std::uint8_t loopCnt = (len / (DMA_MAX_SINGLE_TRANSACTION_PAYLOAD + 1)) + 1;
        std::uint32_t frameSize    = 0;
        std::uint32_t bytesSent    = 0;

        for (std::uint8_t i = 0; i < loopCnt; ++i) {
            /* The MAJOR loop of the DMA can have maximum value of 32767 */
            if (len > DMA_MAX_SINGLE_TRANSACTION_PAYLOAD) {
                frameSize = DMA_MAX_SINGLE_TRANSACTION_PAYLOAD;
            }
            else {
                frameSize = len;
            }

            xfer.rxData      = nullptr;
            xfer.txData      = reinterpret_cast<std::uint8_t *>(txBuffer) + bytesSent;
            xfer.dataSize    = frameSize;
            xfer.configFlags = kLPSPI_MasterByteSwap | kLPSPI_MasterPcsContinuous;

            SCB_CleanInvalidateDCache();
            status = LPSPI_MasterTransferEDMA(lpspiEdmaDriverState.resource->base, lpspiEdmaDriverState.handle, &xfer);
            if (status != kStatus_Success) {
                /* In case of error just flush transfer complete queue */
                xQueueReset(transferCompleteQueue);

                if (csMode == SPICSConfig::Automatic) {
                    writeCS(SPICSState::Set);
                }
                return status;
            }
            else {
                if (xQueueReceive(transferCompleteQueue, &tx_status, pdMS_TO_TICKS(txTimeoutMs)) != pdPASS) {
                    /* Something very bad happened */
                    if (csMode == SPICSConfig::Automatic) {
                        writeCS(SPICSState::Set);
                    }
                    return -1;
                }
            }

            bytesSent += frameSize;
            len -= frameSize;
        }

        if (csMode == SPICSConfig::Automatic) {
            writeCS(SPICSState::Set);
        }
        return tx_status;
    }

    status_t readData(void *rxBuffer, std::uint32_t len, SPICSConfig csMode)
    {
        constexpr auto rxTimeoutMs = 2000U;

        status_t tx_status = -1;
        status_t status;
        lpspi_transfer_t xfer{};

        xfer.txData      = nullptr;
        xfer.rxData      = reinterpret_cast<std::uint8_t *>(rxBuffer);
        xfer.dataSize    = len;
        xfer.configFlags = kLPSPI_MasterByteSwap | kLPSPI_MasterPcsContinuous;

        if (csMode == SPICSConfig::Automatic) {
            writeCS(SPICSState::Clear);
        }

        /* Slow down the SPI clock for the value proper for the read operation */
        changeSpiFrequency(transferReadClockHz);

        lpspiEdmaDriverState.csConfig         = csMode;
        lpspiEdmaDriverState.handle->userData = &lpspiEdmaDriverState;

        /* Clean TX complete queue */
        xQueueReset(transferCompleteQueue);

        /* Clear BUSY Pin IRQ Flag */
        gpio->ClearPortInterrupts(1 << static_cast<std::uint32_t>(BoardDefinitions::EINK_BUSY_PIN));

        /* Enable BUSY Pin IRQ */
        gpio->EnableInterrupt(1 << static_cast<std::uint32_t>(BoardDefinitions::EINK_BUSY_PIN));

        /* Take the BUSY semaphore without timeout just in case the transmission makes the BUSY pin state change. It
         * enables the driver to block then on the busySemaphore until the BUSY pin is deasserted. */
        xSemaphoreTake(busySemaphore, 0);

        status = LPSPI_MasterTransferEDMA(lpspiEdmaDriverState.resource->base, lpspiEdmaDriverState.handle, &xfer);
        if (status != kStatus_Success) {
            /* In case of error just flush transfer complete queue */
            std::uint32_t dummy = 0;
            xQueueReceive(transferCompleteQueue, &dummy, 0);
            if (csMode == SPICSConfig::Automatic) {
                writeCS(SPICSState::Set);
            }
            return status;
        }
        else {
            if (xQueueReceive(transferCompleteQueue, &tx_status, pdMS_TO_TICKS(rxTimeoutMs)) != pdPASS) {
                /* Something very bad happened */
                if (csMode == SPICSConfig::Automatic) {
                    writeCS(SPICSState::Set);
                }
                return -1;
            }
        }

        if (csMode == SPICSConfig::Automatic) {
            writeCS(SPICSState::Set);
        }

        return tx_status;
    }

    BaseType_t busyPinStateChangeHandler()
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        /* Give semaphore only if something is waiting for it */
        if (lpspiEdmaDriverState.eventRegister == BusyWaitEventState::Registered) {
            gpio->DisableInterrupt(1 << static_cast<std::uint32_t>(BoardDefinitions::EINK_BUSY_PIN));

            if (xSemaphoreGiveFromISR(busySemaphore, &xHigherPriorityTaskWoken) != pdPASS) {
                /* We should never reach this point */
            }

            /* Call user defined "eink not busy" event */
            if (lpspiEdmaDriverState.event != nullptr) {
                lpspiEdmaDriverState.event();
            }
        }

        /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
          exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
        __DSB();
#endif

        return xHigherPriorityTaskWoken;
    }
} // namespace bsp::eink
