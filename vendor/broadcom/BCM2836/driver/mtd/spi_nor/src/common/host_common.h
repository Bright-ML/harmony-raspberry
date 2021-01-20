/*
 * Copyright (c) 2013-2019, Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020, Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __HOST_COMMON_H__
#define __HOST_COMMON_H__

#include "sys/types.h"
#include "los_mux.h"

#include "mtd_common.h"
#include "spinor_common.h"
#include "spi_common.h"

#define reg_read(_host, _reg) \
        mtd_readl((UINTPTR)((char *)_host->regbase + (_reg)))

#define reg_write(_host, _value, _reg) \
        mtd_writel((unsigned)(_value), (UINTPTR)((char *)_host->regbase + (_reg)))

#define get_host(_host) \
        if(LOS_OK != LOS_MuxLock(&(_host)->lock, LOS_WAIT_FOREVER)) \
            return -1;

#define put_host(_host) \
        if(LOS_OK != LOS_MuxUnlock(&(_host)->lock)) \
            return -1;
struct spinor_host {
    struct spinor_info *spinor;

    char     *regbase;
    char     *membase;

    void (*set_system_clock)(unsigned clock, int clk_en);
    void (*set_host_addr_mode)(struct spinor_host *host, int enable);

    char *buffer;
    char *dma_buffer;
    char *dma_buffer_bak;

    int num_chip;
    struct spi spi[1];

    LosMux lock;
};

#endif /* End of __HOST_COMMON_H__ */

