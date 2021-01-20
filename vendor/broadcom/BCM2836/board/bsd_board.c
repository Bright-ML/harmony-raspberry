#include "asm/platform.h"
#include "sys/bus.h"
#include "uart_dev.h"
//#include "imx6ull_uart.h"

#include "los_hw_pri.h"



#define UART_IOMEM_COUNT 0xBC

/**************************************************/
/*        UART Device                      */
/**************************************************/
//#if LOSCFG_DRIVERS_UART

#define BAUD_RATE_LOW 9600
#define BAUD_RATE_HIGH 115200

void dma_cache_clean(UINTPTR start, UINTPTR end)
{
    arm_clean_cache_range(start, end);
}
void dma_cache_inv(UINTPTR start, UINTPTR end)
{
    arm_inv_cache_range(start, end);
}

static struct uart_driver_data uart0_dw_driver = { /*lint !e121 !e129 !e10 -esym(528, uart0_pl011_driver)*/
    .num             = 0,
    .baudrate        = BAUD_RATE_HIGH,
    .attr.fifo_rx_en = 1,
    .attr.fifo_tx_en = 1,
//    .flags           = UART_FLG_RD_BLOCK,
};
static struct uart_softc uart0_softc = { /*lint !e10 !e129*/
    .udd = &uart0_dw_driver,
};

#define UART_ADD_DEVICE(dev, unit) \
    do { \
        dev = device_add_child(nexus, "uart", unit); \
        if (dev != NULL) { \
            device_set_softc(dev, &uart ## unit ## _softc); \
        } \
    } while (0)

// callback never be null pointer
static void uart_add_device(add_res_callback_t callback)
{
    device_t uart_dev;
    UART_ADD_DEVICE(uart_dev, 0);
    callback("uart", SYS_RES_MEMORY, 0, UART_REG_ADDR,
        UART_REG_ADDR + UART_IOMEM_COUNT, UART_IOMEM_COUNT);
    callback("uart", SYS_RES_IRQ, 0, NUM_HAL_INTERRUPT_UART0,
        NUM_HAL_INTERRUPT_UART0, 1);

}
//#endif

//#if defined (LOSCFG_DRIVERS_MTD_SPI_NOR_SUNXI)
static void imx6ull_spinor_resource_init(add_res_callback_t callback)
{
    device_add_child(nexus, "sunxi_spi", 0);
    callback("sunxi_spi", SYS_RES_MEMORY, 0, FMC_REG_PBASE, FMC_REG_PBASE + FMC_REG_OFFSIZE, FMC_REG_OFFSIZE);
    //callback("sunxi_spi", SYS_RES_MEMORY, 1, FMC_MEM_PBASE, FMC_MEM_PBASE + FMC_MEM_OFFSIZE, FMC_MEM_OFFSIZE);
}
//#endif

void machine_resource_init(add_res_callback_t callback)
{
    if (callback == NULL) {
        return;
    }

//#if LOSCFG_DRIVERS_UART
    uart_add_device(callback);
//#endif

//#if defined (LOSCFG_DRIVERS_MTD_SPI_NOR_SUNXI)
    imx6ull_spinor_resource_init(callback);
//#endif
}
