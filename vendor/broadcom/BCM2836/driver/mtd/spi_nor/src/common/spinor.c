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

#include "errno.h"
#include "fs/fs.h"
#include "stdio.h"
#include "stdlib.h"
#include "spinor.h"
#include "mtd_common.h"
#include "spinor_common.h"
#include "mtd_dev.h"

struct MtdDev spinor_mtd;
void AddMtdList(char *type, struct MtdDev *mtd);

extern int get_mtd_info(const char *type);

void* GetMtd(const char *type)
{
	(void)type;
	return &spinor_mtd;
}

static int ramnor_erase(struct MtdDev *mtd, UINT64 start, UINT64 len, UINT64 *failAddr)
{
	unsigned char * rambase = (unsigned char *)mtd->priv;

    uint32_t offset = start;
    uint32_t length = len;

    if (offset + length > mtd->size) {
        return -EINVAL;
    }

    if (offset & (mtd->eraseSize - 1)) {
        return -EINVAL;
    }

    if (length & (mtd->eraseSize - 1)) {
        return -EINVAL;
    }

	memset((void *)(rambase+offset), 0xff, length);
    return 0;
}

static int ramnor_write(struct MtdDev *mtd, UINT64 start, UINT64 len, const char *buf)
{
	unsigned char * rambase = (unsigned char *)mtd->priv;
    uint32_t offset = start;
    uint32_t length = len;

    if ((offset + length) > mtd->size) {
        return -EINVAL;
    }

    if (!length) {
        return 0;
    }

    memcpy((void *)(rambase+offset), buf, length);
	return len;
}

static int ramnor_read(struct MtdDev *mtd, UINT64 start, UINT64 len, char *buf)
{
	unsigned char * rambase = (unsigned char *)mtd->priv;
    uint32_t offset = start;
    uint32_t length = len;
	//int i;

    if ((offset + length) > mtd->size) {
		PRINT_RELEASE("%s %s %d, memcpy: 0x%x, 0x%x, 0x%x\n", __FILE__, __FUNCTION__, __LINE__, (unsigned int)buf, (unsigned int)(rambase+start), (unsigned int)len); 	
        return -EINVAL;
    }

    if (!length) {
		PRINT_RELEASE("%s %s %d, memcpy: 0x%x, 0x%x, 0x%x\n", __FILE__, __FUNCTION__, __LINE__, (unsigned int)buf, (unsigned int)(rambase+start), (unsigned int)len); 	
        return 0;
    }

	//PRINT_RELEASE("%s %s %d, memcpy: 0x%x, 0x%x, 0x%x\n", __FILE__, __FUNCTION__, __LINE__, (unsigned int)buf, (unsigned int)(rambase+start), (unsigned int)len);		

    //return spinor->read(spinor, (uint32_t)from, (uint32_t)len, buf);
    memcpy(buf, (void *)(rambase+offset), length);

	return len;
}


void ramnor_register(struct MtdDev *mtd)
{
    //mtd->priv = (void *)DDR_RAMFS_VBASE;

    //mtd->size = DDR_RAMFS_SIZE;
    mtd->eraseSize = 0x10000;

    mtd->type = MTD_NORFLASH;

    mtd->erase = ramnor_erase;
    mtd->read = ramnor_read;
    mtd->write = ramnor_write;

}

/*---------------------------------------------------------------------------*/
/* spinor_node_register- spinor node register */
/*---------------------------------------------------------------------------*/
int spinor_node_register(struct MtdDev *mtd)
{
    int ret = 0;
    ret = register_blockdriver("/dev/spinor", &g_dev_spinor_ops, 0755, mtd);
    if (ret) {
        ERR_MSG("register spinor err %d!\n", ret);
    }

    return ret;
}

int spinor_init(void)
{
    spinor_mtd.priv = (void *)DDR_RAMFS_VBASE;
	spinor_mtd.size = DDR_RAMFS_SIZE;

    /* ramnor register */
    ramnor_register(&spinor_mtd);
	PRINT_RELEASE("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);		
//    AddMtdList("spinor", &spinor_mtd);
    if (spinor_node_register(&spinor_mtd)) {
        PRINT_RELEASE("spinor node register fail!\n");
        return -1;
    }
    return get_mtd_info("spinor") ;
}
