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

#include "stdio.h"
#include "los_typedef.h"
#include "asm/platform.h"
#include "los_base.h"
#include "los_vm_phys.h"
//#include "asm/dma.h"

extern int snprintf_s(char *str, size_t sizeOfBuffer, size_t n, const char *fmt, ...);
/*----------------------------------------------------------------------------*/
/* ulltostr */
/*----------------------------------------------------------------------------*/
char *ulltostr(unsigned long long size)
{
    int ix;
    static char buffer[20];
    unsigned long size_long;
    char *fmt[] = {"%u", "%uK", "%uM", "%uG", "%uT", "%uP"};

    for (ix = 0; (ix < 5) && !(size & 0x3FF) && size; ix++) {
        size = (size >> 10);
    }

    size_long = (unsigned long)size;

    snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, fmt[ix], size_long);

    return buffer;
}

/*----------------------------------------------------------------------------*/
/* ffs */
/*----------------------------------------------------------------------------*/
int ffs(int x)
{
    int r = 1;
    unsigned int f = (unsigned int)x;

    if (!f) {
        return 0;
    }

    if (!(f & 0xffff)) {
        f >>= 16;
        r += 16;
    }
    if (!(f & 0xff)) {
        f >>= 8;
        r += 8;
    }
    if (!(f & 0xf)) {
        f >>= 4;
        r += 4;
    }
    if (!(f & 3)) {
        f >>= 2;
        r += 2;
    }
    if (!(f & 1)) {
        r += 1;
    }
    return r;
}
#if 0
/*----------------------------------------------------------------------------*/
/* mtd_dma_cache_inv */
/*----------------------------------------------------------------------------*/
void mtd_dma_cache_inv(void *addr, unsigned int size)
{
    addr = (void *)(UINTPTR)LOS_PaddrToKVaddr((PADDR_T)(UINTPTR)addr);

    UINTPTR start = (UINTPTR)addr & ~(CACHE_ALIGNED_SIZE - 1);
    UINTPTR end = (UINTPTR)addr + size;

    end = ALIGN(end, CACHE_ALIGNED_SIZE);

    dma_cache_inv(start, end);
}

/*----------------------------------------------------------------------------*/
/* mtd_dma_cache_clean */
/*----------------------------------------------------------------------------*/
void mtd_dma_cache_clean(void *addr, unsigned int size)
{
    addr = (void *)(UINTPTR)LOS_PaddrToKVaddr((PADDR_T)(UINTPTR)addr);
    UINTPTR start = (UINTPTR)addr & ~(CACHE_ALIGNED_SIZE - 1);
    UINTPTR end = (UINTPTR)addr + size;

    end = ALIGN(end, CACHE_ALIGNED_SIZE);

    dma_cache_clean(start, end);
}
#endif
