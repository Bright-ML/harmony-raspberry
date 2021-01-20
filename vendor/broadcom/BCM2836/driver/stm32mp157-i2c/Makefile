include $(LITEOSTOPDIR)/config.mk
include $(LITEOSTOPDIR)/../../drivers/hdf/lite/lite.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS :=  i2c_dev.c i2c_stm32mp157.c

LOCAL_FLAGS :=  -I$(LITEOSTOPDIR)/../../drivers/hdf/frameworks/include/config \
                -I$(LITEOSTOPDIR)/../../drivers/hdf/frameworks/include/core \
                -I$(LITEOSTOPDIR)/../../drivers/hdf/frameworks/ability/sbuf/include \
                -I$(LITEOSTOPDIR)/../../drivers/hdf/frameworks/utils/include \
                -I$(LITEOSTOPDIR)/../../drivers/hdf/frameworks/include/osal \
                -I$(LITEOSTOPDIR)/../../drivers/hdf/frameworks/include/platform \
                -I$(LITEOSTOPDIR)/../../drivers/hdf/lite/include/host/

LOCAL_CFLAGS += -fstack-protector-strong

include $(HDF_DRIVER)

