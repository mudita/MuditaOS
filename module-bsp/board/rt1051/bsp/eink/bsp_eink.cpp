// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "bsp_eink.h"
#include "board.h"
#include "fsl_lpspi.h"
#include "fsl_lpspi_edma.h"
#include "fsl_common.h"
#include "dma_config.h"
#include "bsp/eink/eink_pin_config.hpp"

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

#include "drivers/pll/DriverPLL.hpp"
#include "drivers/dmamux/DriverDMAMux.hpp"
#include "drivers/dma/DriverDMA.hpp"
#include "drivers/gpio/DriverGPIO.hpp"
#include "board/BoardDefinitions.hpp"

#define BSP_EINK_LPSPI_PCS_TO_SCK_DELAY       1000
#define BSP_EINK_LPSPI_SCK_TO_PSC_DELAY       1000
#define BSP_EINK_LPSPI_BETWEEN_TRANSFER_DELAY 1000
#define BSP_EINK_LPSPI_MASTER_PCS_PIN_SEL     (kLPSPI_MasterPcs0)
#define BSP_EINK_LPSPI_SLAVE_PCS_PIN_SEL      (kLPSPI_SlavePcs0)

#define BSP_EINK_LPSPI_BASE BOARD_EINK_LPSPI_BASE

#define BSP_EINK_TRANSFER_WRITE_CLOCK 18000000
#define BSP_EINK_TRANSFER_READ_CLOCK  3000000

typedef enum
{
    EventWaitRegistered,
    EventWaitNotRegistered
} busy_wait_registered_e;

typedef const struct _lpspi_edma_resource
{
    DMA_Type *txEdmaBase;
    uint32_t txEdmaChannel;
    uint8_t txDmaRequest;

    DMA_Type *rxEdmaBase;
    uint32_t rxEdmaChannel;
    uint8_t rxDmaRequest;

#if (defined(FSL_FEATURE_SOC_DMAMUX_COUNT) && FSL_FEATURE_SOC_DMAMUX_COUNT)
    DMAMUX_Type *txDmamuxBase;
    DMAMUX_Type *rxDmamuxBase;
#endif
} lpspi_edma_resource_t;

typedef const struct _lpspi_resource
{
    LPSPI_Type *base;
    uint32_t instance;
    uint32_t (*GetFreq)(void);
} lpspi_resource_t;

typedef struct _bsp_eink_driver
{
    lpspi_resource_t *resource;
    lpspi_edma_resource_t *dmaResource;
    lpspi_master_edma_handle_t *handle;
    edma_handle_t *edmaRxRegToRxDataHandle;
    edma_handle_t *edmaTxDataToTxRegHandle;
    uint32_t baudRate_Bps;
    uint8_t flags; /*!< Control and state flags. */

    bsp_eink_BusyEvent event;
    /* Current transfer chip select configuration( automatic of manual ) */
    eink_spi_cs_config_e chipselectConf;
    /* Tells if something is waiting for not busy event */
    busy_wait_registered_e eventRegister;

} bsp_eink_driver_t;

using namespace drivers;
static lpspi_master_config_t s_eink_lpspi_master_config;
static std::shared_ptr<drivers::DriverGPIO> gpio;
static std::shared_ptr<drivers::DriverPLL> pll;
static std::shared_ptr<drivers::DriverDMA> dma;
static std::shared_ptr<drivers::DriverDMAMux> dmamux;
static std::unique_ptr<drivers::DriverDMAHandle> rxDMAHandle;
static std::unique_ptr<drivers::DriverDMAHandle> txDMAHandle;

static uint32_t BSP_EINK_LPSPI_GetFreq(void)
{
    return GetPerphSourceClock(PerphClock_LPSPI);
}

lpspi_resource_t BSP_EINK_LPSPI_Resource = {BSP_EINK_LPSPI_BASE, 1, BSP_EINK_LPSPI_GetFreq};

lpspi_edma_resource_t BSP_EINK_LPSPI_EdmaResource = {
    BSP_EINK_LPSPI_DMA_TX_DMA_BASE,
    BSP_EINK_LPSPI_DMA_TX_CH,
    BSP_EINK_LPSPI_DMA_TX_PERI_SEL,
    BSP_EINK_LPSPI_DMA_RX_DMA_BASE,
    BSP_EINK_LPSPI_DMA_RX_CH,
    BSP_EINK_LPSPI_DMA_RX_PERI_SEL,

    BSP_EINK_LPSPI_DMA_TX_DMAMUX_BASE,
    BSP_EINK_LPSPI_DMA_RX_DMAMUX_BASE,
};

AT_NONCACHEABLE_SECTION(lpspi_master_edma_handle_t BSP_EINK_LPSPI_EdmaHandle);

static bsp_eink_driver_t BSP_EINK_LPSPI_EdmaDriverState = {
    &BSP_EINK_LPSPI_Resource,
    &BSP_EINK_LPSPI_EdmaResource,
    &BSP_EINK_LPSPI_EdmaHandle,
    nullptr, // will be filled in init function
    nullptr, // will be filled in init function
    0,
    0,
    0,
    SPI_AUTOMATIC_CS,
    EventWaitRegistered,
};

static SemaphoreHandle_t bsp_eink_TransferComplete;

static SemaphoreHandle_t bsp_eink_busySemaphore; //  This semaphore suspends the task until the EPD display is busy

static void s_LPSPI_MasterEdmaCallback(LPSPI_Type *base,
                                       lpspi_master_edma_handle_t *handle,
                                       status_t status,
                                       void *userData)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // bsp_eink_driver_t* driver = (bsp_eink_driver_t*)userData;

    if (xQueueSendFromISR(bsp_eink_TransferComplete, &status, &xHigherPriorityTaskWoken) != pdPASS) {
        // if here sth really bad happened
        return;
    }

    // Disable SPI for current consumption reducing
    LPSPI_Enable(BSP_EINK_LPSPI_BASE, false);

    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

status_t BSP_EinkInit(bsp_eink_BusyEvent event)
{
    bsp_eink_driver_t *lpspi = &BSP_EINK_LPSPI_EdmaDriverState;
    // lpspi_edma_resource_t *dmaResource = lpspi->dmaResource;

    // If was already created - free it
    if (bsp_eink_busySemaphore != NULL) {
        vSemaphoreDelete(bsp_eink_busySemaphore);
    }

    // Create new semaphore
    bsp_eink_busySemaphore = xSemaphoreCreateBinary();
    if (bsp_eink_busySemaphore == NULL) {
        return kStatus_Fail;
    }

    // At the beginning the semaphore is in unknown state and it has to clearly taken or given
    xSemaphoreTake(bsp_eink_busySemaphore, 0);

    // If was already created - free it
    if (bsp_eink_TransferComplete != NULL) {
        vQueueDelete(bsp_eink_TransferComplete);
    }

    // Create new queue
    bsp_eink_TransferComplete = xQueueCreate(1, sizeof(uint32_t));
    if (bsp_eink_TransferComplete == NULL) {
        vSemaphoreDelete(bsp_eink_busySemaphore);
        bsp_eink_busySemaphore = NULL;
        return kStatus_Fail;
    }

    lpspi->eventRegister = EventWaitNotRegistered;

    gpio = DriverGPIO::Create(static_cast<GPIOInstances>(BoardDefinitions::EINK_GPIO), DriverGPIOParams{});

    bsp::eink::eink_configure_gpio_pins();

    s_eink_lpspi_master_config.baudRate     = BSP_EINK_TRANSFER_WRITE_CLOCK;
    s_eink_lpspi_master_config.bitsPerFrame = 8;
    s_eink_lpspi_master_config.cpol         = kLPSPI_ClockPolarityActiveHigh;
    s_eink_lpspi_master_config.cpha         = kLPSPI_ClockPhaseFirstEdge;
    s_eink_lpspi_master_config.direction    = kLPSPI_MsbFirst;

    s_eink_lpspi_master_config.pcsToSckDelayInNanoSec        = 1000000000 / s_eink_lpspi_master_config.baudRate;
    s_eink_lpspi_master_config.lastSckToPcsDelayInNanoSec    = 1000000000 / s_eink_lpspi_master_config.baudRate;
    s_eink_lpspi_master_config.betweenTransferDelayInNanoSec = 1000000000 / s_eink_lpspi_master_config.baudRate;

    s_eink_lpspi_master_config.whichPcs           = kLPSPI_Pcs0;
    s_eink_lpspi_master_config.pcsActiveHighOrLow = kLPSPI_PcsActiveLow;

    s_eink_lpspi_master_config.pinCfg        = kLPSPI_SdiInSdoOut;
    s_eink_lpspi_master_config.dataOutConfig = kLpspiDataOutRetained;

    LPSPI_MasterInit(BSP_EINK_LPSPI_BASE, &s_eink_lpspi_master_config, GetPerphSourceClock(PerphClock_LPSPI));

    // fsl_lpspi doesn't support configuring autopcs feature
    BSP_EINK_LPSPI_BASE->CFGR1 |= LPSPI_CFGR1_AUTOPCS(0);

    dmamux = DriverDMAMux::Create(static_cast<DMAMuxInstances>(BoardDefinitions::EINK_DMAMUX), DriverDMAMuxParams{});
    dma    = DriverDMA::Create(static_cast<DMAInstances>(BoardDefinitions::EINK_DMA), DriverDMAParams{});

    txDMAHandle = dma->CreateHandle(static_cast<uint32_t>(BoardDefinitions::EINK_TX_DMA_CHANNEL));
    rxDMAHandle = dma->CreateHandle(static_cast<uint32_t>(BoardDefinitions::EINK_RX_DMA_CHANNEL));
    dmamux->Enable(static_cast<uint32_t>(BoardDefinitions::EINK_TX_DMA_CHANNEL),
                   BSP_EINK_LPSPI_DMA_TX_PERI_SEL); // TODO: M.P fix BSP_EINK_LPSPI_DMA_TX_PERI_SEL
    dmamux->Enable(static_cast<uint32_t>(BoardDefinitions::EINK_RX_DMA_CHANNEL),
                   BSP_EINK_LPSPI_DMA_RX_PERI_SEL); // TODO: M.P fix BSP_EINK_LPSPI_DMA_RX_PERI_SEL

    BSP_EINK_LPSPI_EdmaDriverState.edmaTxDataToTxRegHandle =
        reinterpret_cast<edma_handle_t *>(txDMAHandle->GetHandle());
    BSP_EINK_LPSPI_EdmaDriverState.edmaRxRegToRxDataHandle =
        reinterpret_cast<edma_handle_t *>(rxDMAHandle->GetHandle());

    LPSPI_SetMasterSlaveMode(BSP_EINK_LPSPI_BASE, kLPSPI_Master);
    LPSPI_MasterTransferCreateHandleEDMA(BSP_EINK_LPSPI_BASE,
                                         lpspi->handle,
                                         s_LPSPI_MasterEdmaCallback,
                                         NULL,
                                         lpspi->edmaRxRegToRxDataHandle,
                                         lpspi->edmaTxDataToTxRegHandle);

    return kStatus_Success;
}

status_t BSP_EinkChangeSpiFrequency(uint32_t frequencyHz)
{
    uint32_t tcrPrescalerValue = 0;

    LPSPI_Enable(BSP_EINK_LPSPI_BASE, false);
    LPSPI_MasterSetBaudRate(
        BSP_EINK_LPSPI_BASE, frequencyHz, GetPerphSourceClock(PerphClock_LPSPI), &tcrPrescalerValue);

    s_eink_lpspi_master_config.baudRate = frequencyHz;
    BSP_EINK_LPSPI_BASE->TCR =
        LPSPI_TCR_CPOL(s_eink_lpspi_master_config.cpol) | LPSPI_TCR_CPHA(s_eink_lpspi_master_config.cpha) |
        LPSPI_TCR_LSBF(s_eink_lpspi_master_config.direction) |
        LPSPI_TCR_FRAMESZ(s_eink_lpspi_master_config.bitsPerFrame - 1) | LPSPI_TCR_PRESCALE(tcrPrescalerValue) |
        LPSPI_TCR_PCS(s_eink_lpspi_master_config.whichPcs);

    return 0;
}

void BSP_EinkDeinit(void)
{
    LPSPI_Enable(BSP_EINK_LPSPI_BASE, false);

    if (bsp_eink_busySemaphore != NULL) {
        vSemaphoreDelete(bsp_eink_busySemaphore);
        bsp_eink_busySemaphore = NULL;
    }

    if (bsp_eink_TransferComplete != NULL) {
        vQueueDelete(bsp_eink_TransferComplete);
        bsp_eink_TransferComplete = NULL;
    }

    pll.reset();

    dmamux->Disable(static_cast<uint32_t>(BoardDefinitions::EINK_TX_DMA_CHANNEL));
    dmamux->Disable(static_cast<uint32_t>(BoardDefinitions::EINK_RX_DMA_CHANNEL));

    dma.reset();
    dmamux.reset();

    gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::EINK_BUSY_PIN));
    gpio->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::EINK_BUSY_PIN));
    gpio.reset();

    // pll.reset(); TODO:M.P
}

status_t BSP_EinkWriteData(void *txBuffer, uint32_t len, eink_spi_cs_config_e cs)
{
    const uint32_t TX_TIMEOUT_MS = 2000;
    status_t tx_status           = 0;
    status_t status;
    lpspi_transfer_t xfer = {};

    if (cs == SPI_AUTOMATIC_CS) {
        BSP_EinkWriteCS(BSP_Eink_CS_Clr);
    }

    uint8_t loopCnt    = (len / (DMA_MAX_SINGLE_TRANSACTION_PAYLOAD + 1)) + 1;
    uint32_t frameSize = 0;
    uint32_t bytesSent = 0;

    // Increase the SPI frequency to the SPI WRITE value
    BSP_EinkChangeSpiFrequency(BSP_EINK_TRANSFER_WRITE_CLOCK);

    BSP_EINK_LPSPI_EdmaDriverState.chipselectConf   = cs;
    BSP_EINK_LPSPI_EdmaDriverState.handle->userData = &BSP_EINK_LPSPI_EdmaDriverState;

    // Clean the TX complete queue
    xQueueReset(bsp_eink_TransferComplete);
    // Clear the BUSY Pin IRQ Flag
    gpio->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::EINK_BUSY_PIN));
    // Enable the BUSY Pin IRQ
    gpio->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::EINK_BUSY_PIN));
    // Take the BUSY semaphore without timeout just in case the transmission makes the BUSY pin state change. It
    // enables the driver to block then on the bsp_eink_busySemaphore until the BUSY pin is deasserted
    xSemaphoreTake(bsp_eink_busySemaphore, 0);

    // The MAJOR loop of the DMA can be maximum of value 32767
    for (uint8_t i = 0; i < loopCnt; ++i) {
        if (len > DMA_MAX_SINGLE_TRANSACTION_PAYLOAD) {
            frameSize = DMA_MAX_SINGLE_TRANSACTION_PAYLOAD;
        }
        else {
            frameSize = len;
        }

        xfer.rxData      = NULL;
        xfer.txData      = (uint8_t *)txBuffer + bytesSent;
        xfer.dataSize    = frameSize;
        xfer.configFlags = /*RTE_SPI1_MASTER_PCS_PIN_SEL |*/ kLPSPI_MasterByteSwap | kLPSPI_MasterPcsContinuous;

        SCB_CleanInvalidateDCache();
        status = LPSPI_MasterTransferEDMA(
            BSP_EINK_LPSPI_EdmaDriverState.resource->base, BSP_EINK_LPSPI_EdmaDriverState.handle, &xfer);

        if (status != kStatus_Success) {
            // in case of error just flush transfer complete queue
            xQueueReset(bsp_eink_TransferComplete);

            if (cs == SPI_AUTOMATIC_CS) {
                BSP_EinkWriteCS(BSP_Eink_CS_Set);
            }

            return status;
        }
        else {
            if (xQueueReceive(bsp_eink_TransferComplete, &tx_status, pdMS_TO_TICKS(TX_TIMEOUT_MS)) != pdPASS) {
                // sth really bad happened
                if (cs == SPI_AUTOMATIC_CS) {
                    BSP_EinkWriteCS(BSP_Eink_CS_Set);
                }

                return -1;
            }
        }

        bytesSent += frameSize;
        len -= frameSize;
    }

    if (cs == SPI_AUTOMATIC_CS) {
        BSP_EinkWriteCS(BSP_Eink_CS_Set);
    }

    return tx_status;
}

status_t BSP_EinkReadData(void *rxBuffer, uint32_t len, eink_spi_cs_config_e cs)
{
    const int RX_TIMEOUT_MS = 2000;
    status_t tx_status      = 0;
    status_t status;
    lpspi_transfer_t xfer = {};

    xfer.txData      = NULL;
    xfer.rxData      = (uint8_t *)rxBuffer;
    xfer.dataSize    = len;
    xfer.configFlags = /*RTE_SPI1_MASTER_PCS_PIN_SEL |*/ kLPSPI_MasterByteSwap | kLPSPI_MasterPcsContinuous;

    if (cs == SPI_AUTOMATIC_CS) {
        BSP_EinkWriteCS(BSP_Eink_CS_Clr);
    }

    // Slow down the SPI clock for the value proper for the read operation
    BSP_EinkChangeSpiFrequency(BSP_EINK_TRANSFER_READ_CLOCK);

    BSP_EINK_LPSPI_EdmaDriverState.chipselectConf   = cs;
    BSP_EINK_LPSPI_EdmaDriverState.handle->userData = &BSP_EINK_LPSPI_EdmaDriverState;

    // Clean the TX complete queue
    xQueueReset(bsp_eink_TransferComplete);
    // Clear the BUSY Pin IRQ Flag
    gpio->ClearPortInterrupts(1 << static_cast<uint32_t>(BoardDefinitions::EINK_BUSY_PIN));
    // Enable the BUSY Pin IRQ
    gpio->EnableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::EINK_BUSY_PIN));
    // Take the BUSY semaphore without timeout just in case the transmission makes the BUSY pin state change. It
    // enables the driver to block then on the bsp_eink_busySemaphore until the BUSY pin is deasserted
    xSemaphoreTake(bsp_eink_busySemaphore, 0);

    status = LPSPI_MasterTransferEDMA(
        BSP_EINK_LPSPI_EdmaDriverState.resource->base, BSP_EINK_LPSPI_EdmaDriverState.handle, &xfer);
    if (status != kStatus_Success) {
        // in case of error just flush transfer complete queue
        uint32_t dummy = 0;
        xQueueReceive(bsp_eink_TransferComplete, &dummy, 0);
        if (cs == SPI_AUTOMATIC_CS) {
            BSP_EinkWriteCS(BSP_Eink_CS_Set);
        }

        return status;
    }
    else {
        if (xQueueReceive(bsp_eink_TransferComplete, &tx_status, pdMS_TO_TICKS(RX_TIMEOUT_MS)) != pdPASS) {
            // sth really bad happened
            if (cs == SPI_AUTOMATIC_CS) {
                BSP_EinkWriteCS(BSP_Eink_CS_Set);
            }

            return -1;
        }
    }

    if (cs == SPI_AUTOMATIC_CS) {
        BSP_EinkWriteCS(BSP_Eink_CS_Set);
    }

    return tx_status;
}

uint8_t BSP_EinkWaitUntilDisplayBusy(uint32_t timeout)
{
    uint8_t ret                                  = 0;
    BSP_EINK_LPSPI_EdmaDriverState.eventRegister = EventWaitRegistered;
    if (xSemaphoreTake(bsp_eink_busySemaphore, timeout) != pdPASS) {
        ret = 0;
    }
    else {
        xSemaphoreGive(bsp_eink_busySemaphore);
        ret = 1;
    }

    BSP_EINK_LPSPI_EdmaDriverState.eventRegister = EventWaitNotRegistered;
    return ret;
}

void BSP_EinkResetDisplayController(void)
{
    BSP_EinkWriteCS(BSP_Eink_CS_Set);

    gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::EINK_RESET_PIN), 0);
    vTaskDelay(10);
    gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::EINK_RESET_PIN), 1);
    vTaskDelay(10);
}

void BSP_EinkWriteCS(bsp_eink_cs_ctrl_t ctrl)
{
    if (ctrl == BSP_Eink_CS_Clr) {
        gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::EINK_CS_PIN), 0);
    }
    else if (ctrl == BSP_Eink_CS_Set) {
        gpio->WritePin(static_cast<uint32_t>(BoardDefinitions::EINK_CS_PIN), 1);
    }
    else {
        return;
    }
}

BaseType_t BSP_EinkBusyPinStateChangeHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* Give semaphore only if something is waiting on it */
    if (BSP_EINK_LPSPI_EdmaDriverState.eventRegister == EventWaitRegistered) {
        gpio->DisableInterrupt(1 << static_cast<uint32_t>(BoardDefinitions::EINK_BUSY_PIN));

        if (xSemaphoreGiveFromISR(bsp_eink_busySemaphore, &xHigherPriorityTaskWoken) != pdPASS) {
            // shouldn't get here!
        }

        // call user defined "eink not busy" event
        if (BSP_EINK_LPSPI_EdmaDriverState.event) {
            BSP_EINK_LPSPI_EdmaDriverState.event();
        }
    }

    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif

    return xHigherPriorityTaskWoken;
}
