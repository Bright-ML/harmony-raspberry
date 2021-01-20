# Copyright (c) 2020 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


    LITEOS_BASELIB += -lhdf_config
    LIB_SUBDIRS += $(LITEOS_SOURCE_ROOT)/vendor/broadcom/$(LITEOS_PLATFORM)/config


VENDOR_HDF_DRIVERS_ROOT := $(LITEOSTOPDIR)/../../vendor/broadcom/hdf

# ifeq ($(LOSCFG_PLATFORM_RASPBERRYPI2B), y)
# LIB_SUBDIRS             += $(BCM2836_BASE_DIR)/driver/stm32mp157-i2c
# LITEOS_BASELIB  += -lstm32mp157-i2c
# endif

# LIB_SUBDIRS             += $(BCM2836_BASE_DIR)/driver/touch
# LITEOS_BASELIB  += -ltouch


# LIB_SUBDIRS             += $(BCM2836_BASE_DIR)/driver/hello
# LITEOS_BASELIB  += -lhello

# lib path
LITEOS_LD_PATH += -L$(VENDOR_HDF_DRIVERS_ROOT)/libs/$(LITEOS_PLATFORM)

