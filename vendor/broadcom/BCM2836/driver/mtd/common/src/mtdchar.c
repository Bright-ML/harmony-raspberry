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

#include "los_config.h"
#if defined(LOSCFG_FS_YAFFS) || defined(LOSCFG_FS_JFFS)

#include "fs/fs.h"
#include "stdio.h"
#include "string.h"
#include "errno.h"

#include "los_mux.h"
#include "mtd_dev.h"

#include "mtd_partition.h"
#include "user_copy.h"

/*
 * open device interface
 */
static int mtdchar_open(FAR struct file *filep)
{
    struct inode *inode_p = filep->f_inode;
    mtd_partition *partition = (mtd_partition *)(inode_p->i_private);

    if (partition->user_num != 0) { // be opened
        return -EBUSY;
    }

    struct MtdDev *mtd = (struct MtdDev *)(partition->mtd_info);
    size_t block_size = mtd->eraseSize;

    (void)LOS_MuxLock(&partition->lock, LOS_WAIT_FOREVER);

    partition->user_num = 1;
    filep->f_pos = partition->start_block * block_size;

    (void)LOS_MuxUnlock(&partition->lock);

    return ENOERR;
}

/*
 * close device interface
 */
static int mtdchar_close(FAR struct file *filep)
{
    struct inode *inode_p = filep->f_inode;
    mtd_partition *partition = (mtd_partition *)(inode_p->i_private);

    (void)LOS_MuxLock(&partition->lock, LOS_WAIT_FOREVER);

    partition->user_num = 0;

    (void)LOS_MuxUnlock(&partition->lock);

    return ENOERR;
}

/*
 * read device interface
 */
static ssize_t mtdchar_read(FAR struct file *filep, FAR char *buffer, size_t buflen)
{
    struct inode *inode_p = filep->f_inode;
    mtd_partition *partition = (mtd_partition *)(inode_p->i_private);

    (void)LOS_MuxLock(&partition->lock, LOS_WAIT_FOREVER);

    struct MtdDev *mtd = (struct MtdDev *)(partition->mtd_info);
    uint64_t block_size = mtd->eraseSize;
    uint64_t start_addr = partition->start_block * block_size;
    uint64_t end_addr = (partition->end_block + 1) * block_size;

    uint64_t retlen;
    ssize_t ret = 0;


    if (!buflen) {
        ret = 0;
        goto out1;
    }

    retlen = mtd->read(mtd, start_addr, end_addr - start_addr, buffer);

    if (retlen < 0) {
        goto out1;
    }

    filep->f_pos += retlen;

    ret = (ssize_t)retlen;

out1:
    (void)LOS_MuxUnlock(&partition->lock);
    return ret;
}

/*
 * write device interface
 */
static ssize_t mtdchar_write(FAR struct file *filep, FAR const char *buffer, size_t buflen)
{
    struct inode *inode_p = filep->f_inode;
    mtd_partition *partition = (mtd_partition *)(inode_p->i_private);

    (void)LOS_MuxLock(&partition->lock, LOS_WAIT_FOREVER);

    struct MtdDev *mtd = (struct MtdDev *)(partition->mtd_info);
    uint64_t block_size = mtd->eraseSize;
    uint64_t start_addr = partition->start_block * block_size;
    uint64_t end_addr = (partition->end_block + 1) * block_size;
    uint64_t retlen;
    int ret = 0;

    if (!buflen) {
        ret = 0;
        goto out1;
    }

    retlen = mtd->write(mtd, start_addr, end_addr - start_addr, buffer);

    if (retlen < 0) {
        goto out1;
    }

    filep->f_pos += retlen;

    ret = (ssize_t)retlen;

out1:
    (void)LOS_MuxUnlock(&partition->lock);
    return ret;
}

/*
 * lseek device interface
 */
static off_t mtdchar_lseek(FAR struct file *filep, off_t offset, int whence)
{
    struct inode *inode_p = filep->f_inode;
    mtd_partition *partition = (mtd_partition *)(inode_p->i_private);

    (void)LOS_MuxLock(&partition->lock, LOS_WAIT_FOREVER);

    struct MtdDev *mtd = (struct MtdDev *)(partition->mtd_info);
    size_t block_size = mtd->eraseSize;
    size_t end_addr = (partition->end_block + 1) * block_size;
    size_t start_addr = partition->start_block * block_size;

    switch (whence) {
        case SEEK_SET:
            if (offset >= 0 && (size_t)offset < end_addr - start_addr) {
                filep->f_pos = start_addr + offset;
                goto out1;
            } else {
                goto err1;
            }

        case SEEK_CUR:
            if (offset + (size_t)filep->f_pos >= start_addr &&
                    (size_t)(offset + filep->f_pos) < end_addr) {
                filep->f_pos += offset;
                goto out1;
            } else {
                goto err1;
            }

        case SEEK_END:
            if (offset < 0 && offset + end_addr >= start_addr) {
                filep->f_pos = (off_t)(offset + end_addr);
                goto out1;
            } else {
                goto err1;
            }

        default:
            goto err1;
    }
err1:
    (void)LOS_MuxUnlock(&partition->lock);
    return -EINVAL;
out1:
    (void)LOS_MuxUnlock(&partition->lock);
    return filep->f_pos;
}

static ssize_t mtdchar_map(FAR struct file* filep, FAR LosVmMapRegion *region)
{
    PRINTK("%s %d, mmap is not support\n", __FUNCTION__, __LINE__);
    return 0;
}

const struct file_operations_vfs g_mtdchar_fops = {
    .open   =   mtdchar_open,
    .close  =   mtdchar_close,
    .read   =   mtdchar_read,
    .write  =   mtdchar_write,
    .seek   =   mtdchar_lseek,
    .mmap   =   mtdchar_map,
#ifndef CONFIG_DISABLE_POLL
    .poll   =   NULL,
#endif
    .unlink =   NULL,
};

#endif
