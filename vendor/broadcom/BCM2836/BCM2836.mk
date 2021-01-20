
BCM2836_BASE_DIR  := $(LITEOSTOPDIR)/../../vendor/broadcom/BCM2836

LIB_SUBDIRS     += $(BCM2836_BASE_DIR)/board
LITEOS_BASELIB  += -lboard

LIB_SUBDIRS     += $(BCM2836_BASE_DIR)/driver/mtd/common
LITEOS_BASELIB  += -lmtd_common

LIB_SUBDIRS     += $(BCM2836_BASE_DIR)/driver/mtd/spi_nor
LITEOS_BASELIB  += -lspinor_flash

ifeq ($(LOSCFG_DRIVERS_VIDEO), y)
LIB_SUBDIRS             += $(BCM2836_BASE_DIR)/driver/stm32mp157-fb
LITEOS_BASELIB  += -lstm32mp157-fb
endif

LIB_SUBDIRS             += $(BCM2836_BASE_DIR)/driver/uart
LITEOS_BASELIB  += -luart


LITEOS_MTD_SPI_NOR_INCLUDE +=   -I$(BCM2836_BASE_DIR)/driver/mtd/common/include \
                                                                -I$(BCM2836_BASE_DIR)/driver/mtd/spi_nor/include

