

/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: ft6236 touch driver implement.
 * Author: zhaihaipeng
 * Create: 2020-07-25
 */

#include <stdlib.h>
#include <asm/io.h>
#include <fs/fs.h>
#include <fs_poll_pri.h>
#include <los_queue.h>
#include <poll.h>
#include <user_copy.h>
#include <securec.h>
#include "gpio_if.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "osal_irq.h"
#include "osal_mem.h"
#include "osal_time.h"


static int g_hello_val = 0x12345678;


static int hello_open(FAR struct file *filep)
{
    HDF_LOGI("%s: called", __func__);
    if (filep == NULL) {
        HDF_LOGE("%s: fliep is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

static int hello_close(FAR struct file *filep)
{
    HDF_LOGI("%s: called", __func__);

    if (filep == NULL) {
        HDF_LOGE("%s: fliep is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

static ssize_t hello_read(FAR struct file *filep, FAR char *buffer, size_t buflen)
{
	if (buflen == 4) {
		LOS_ArchCopyToUser(buffer, &g_hello_val, 4);
		return 4;
	}
	return 0;
}

static const struct file_operations_vfs g_helloDevOps = {
    .open   = hello_open,
    .close  = hello_close,
    .read   = hello_read,
    .write  = NULL,
    .seek   = NULL,
    .ioctl  = NULL,
    .mmap   = NULL,
    .unlink = NULL,
};


int32_t hello_dispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmdId;
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_FAILURE;
    }

	if (!HdfSbufWriteInt32(reply, g_hello_val))
	{
        HDF_LOGE("%s: reply int32 fail", __func__);
	}
	
    return HDF_SUCCESS;
}

int32_t hello_bind(struct HdfDeviceObject *object)
{
    if (object == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    static struct IDeviceIoService service = {
        .object = {0},
        .Dispatch = hello_dispatch,
    };
    object->service = &service;
    return HDF_SUCCESS;
}

int hello_init(struct HdfDeviceObject *object)
{
    (void)object;
    HDF_LOGI("%s: enter", __func__);
    int ret = register_driver("/dev/hello", &g_helloDevOps, 0666, NULL);
    if (ret != 0) {
        HDF_LOGE("%s: register hello dev failed, ret %d", __func__, ret);
        return HDF_FAILURE;
    }
    HDF_LOGI("%s: exit succ", __func__);
    return HDF_SUCCESS;

}

struct HdfDriverEntry g_HelloDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_PLATFORM_HELLO",
    .Bind = hello_bind,
    .Init = hello_init,
};

HDF_INIT(g_HelloDevEntry);

