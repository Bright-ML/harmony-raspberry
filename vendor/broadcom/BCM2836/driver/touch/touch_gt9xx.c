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
#include "touch_gt9xx.h"


/* device node path and access right */
#define TOUCH_DEVICE "/dev/input/event1"
#define TOUCH_DEVICE_MODE 0666
/* task config */
#define TASK_PRIO_LEVEL_TWO 2
#define TASK_SIZE 0x6000
#define TOUCH_EVENT_DOWN       0
#define TOUCH_EVENT_UP         1
#define TOUCH_EVENT_CONTACT    2

/* the macro defines of GT911 */
#define ONE_BYTE_MASK         0xFF
#define ONE_BYTE_OFFSET       8
#define GT_EVENT_UP           0x80
#define GT_EVENT_INVALID      0
#define GT_EVENT_SIZE         6
#define GT_X_LOW              0
#define GT_X_HIGH             1
#define GT_Y_LOW              2
#define GT_Y_HIGH             3
#define GT_PRESSURE_LOW       4
#define GT_PRESSURE_HIGH      5
#define GT_ADDR_LEN           2
#define GT_BUF_STATE_ADDR     0x814E
#define GT_X_LOW_BYTE_BASE    0x8150
#define GT_FINGER_NUM_MASK    0x03
#define GT_CLEAN_DATA_LEN     3
#define GT_REG_HIGH_POS       0
#define GT_REG_LOW_POS        1
#define GT_CLEAN_POS          2
#define GT_CLEAN_FLAG         0x0
/* Config info macro of GT911 */
#define GT_CFG_INFO_ADDR      0x8140
#define GT_CFG_INFO_LEN       10
#define GT_PROD_ID_1ST        0
#define GT_PROD_ID_2ND        1
#define GT_PROD_ID_3RD        2
#define GT_PROD_ID_4TH        3
#define GT_FW_VER_LOW         4
#define GT_FW_VER_HIGH        5
#define GT_SOLU_X_LOW         6
#define GT_SOLU_X_HIGH        7
#define GT_SOLU_Y_LOW         8
#define GT_SOLU_Y_HIGH        9

/* the sleep time for task */
#define TASK_SLEEP_MS 100
#define EVENT_SYNC 0x1

static TouchCoreData *g_coreData;
static InputEventData g_touchEventData;
static EVENT_CB_S g_touchEventIrq;

uint32_t IrqHandle(uint32_t irqId, void *dev);


/* start for imx6ull */

/*
 * int pin: GPIO1_IO05
 * rst pin: SNVS_TAMPER2/GPIO5_IO02
 */

#define GT9XX_INT_NUM  (66 + 32)

/** GPIO - Register Layout Typedef */
typedef struct {
  volatile uint32_t DR;                                /**< GPIO data register, offset: 0x0 */
  volatile uint32_t GDIR;                              /**< GPIO direction register, offset: 0x4 */
  volatile  uint32_t PSR;                               /**< GPIO pad status register, offset: 0x8 */
  volatile uint32_t ICR1;                              /**< GPIO interrupt configuration register1, offset: 0xC */
  volatile uint32_t ICR2;                              /**< GPIO interrupt configuration register2, offset: 0x10 */
  volatile uint32_t IMR;                               /**< GPIO interrupt mask register, offset: 0x14 */
  volatile uint32_t ISR;                               /**< GPIO interrupt status register, offset: 0x18 */
  volatile uint32_t EDGE_SEL;                          /**< GPIO edge select register, offset: 0x1C */
} GPIO_Type;


static volatile unsigned int *CCM_CCGR1                              ;
static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER2;
static volatile unsigned int *IOMUXC_GPIO1_IO05_GPIO1_IO05           ;
static volatile unsigned int *IOMUXC_PAD_CTL_GPIO1_IO05              ;
static GPIO_Type *gpio1;
static GPIO_Type *gpio5;

static void gt911_io_init(void)
{
	unsigned int val;
	
	CCM_CCGR1                               = (volatile unsigned int *)IO_DEVICE_ADDR(0x20C406C);
	IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER2 = (volatile unsigned int *)IO_DEVICE_ADDR(0x2290010);
	IOMUXC_GPIO1_IO05_GPIO1_IO05            = (volatile unsigned int *)IO_DEVICE_ADDR(0x20E0070);
	IOMUXC_PAD_CTL_GPIO1_IO05               = (volatile unsigned int *)IO_DEVICE_ADDR(0x20E02FC);	

	gpio1 = (GPIO_Type *)IO_DEVICE_ADDR(0x0209C000);
	gpio5 = (GPIO_Type *)IO_DEVICE_ADDR(0x020AC000);

	/* GPIO5和GPIO1都是使用CCM_CCGR1 */
	/* 使能GPIO5 GPIO1
	 * set CCM to enable GPIO5 GPIO1
	 * CCM_CCGR1[CG15] 0x20C406C
	 * bit[31:30] = 0b11 || bit[27:26] = 0b11 = 0b110011 = 0d51
	 */
	*CCM_CCGR1 |= (51<<26);
	
	/* 设置GPIO5_IO02用于GPIO 同理设置GPIO1_IO05
	 * set IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER2
	 *      to configure GPIO5_IO02 as GPIO
	 * IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER2  0x2290010
	 * bit[3:0] = 0b0101 alt5
	 */
	val = *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER2;
	val &= ~(0xf);
	val |= (5);
	*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER2 = val;
	
	val = *IOMUXC_GPIO1_IO05_GPIO1_IO05;
	val &= ~(0xf);
	val |= (5);
	*IOMUXC_GPIO1_IO05_GPIO1_IO05 = val;

	gpio5->GDIR |= (1<<2);
	gpio1->GDIR |= (1<<5);
}

void gt9xx_irq_init(TouchCoreData *cd)
{
	int32_t ret;

	/* if set 0, masked */
	gpio1->IMR &= ~(1 << 1);
	
	/* if set detects any edge on the corresponding input signal*/
	gpio1->EDGE_SEL &= ~(1 << 5);

	/* falling-edge */
	gpio1->ICR1 |= (3<<10);
	
	/* clear interrupt first to avoid unexpected event */
	gpio1->ISR |= (1 << 5);

	/* IrqHandle */
	ret = OsalRegisterIrq(cd->intGpioNum, 0, IrqHandle, "gt9xx_irq", NULL);

	if (ret)
	{
		HDF_LOGE("%s %s %d, gt9xx_irq_init err\n", __FILE__, __FUNCTION__, __LINE__);
	}
}

static int32_t gt9xx_irq_enable(uint32_t irq)
{
	(void)irq;
	
	/* if set 1, unmasked, Interrupt n is enabled */
	gpio1->IMR |= (1 << 5);

	return HDF_SUCCESS;
}

int32_t gt9xx_irq_disable(uint32_t irq)
{
	(void)irq;
	
	/* if set 0, masked */
	gpio1->IMR &= ~(1 << 5);

	return HDF_SUCCESS;
}

static void gt9xx_irq_clear(uint32_t irq)
{
	(void)irq;
	
	gpio1->ISR |= (1 << 5);
}

static void gt9xx_init(void)
{
    /*初始化gt911_io*/
	gt911_io_init();
	/*初始化两个I0都为低电平*/
	gpio5->DR &= ~(1<<2);
	gpio1->DR &= ~(1<<5);
	/*10ms*/
	OsalMSleep(RESET_LOW_DELAY);
	/*reset 输出高，INT转为悬浮输出态*/
	gpio5->DR |= (1<<2);
	*IOMUXC_PAD_CTL_GPIO1_IO05 = 0x0088; 
	gpio1->GDIR &= ~(1<<5);
	/*100ms*/
	OsalMSleep(RESET_LOW_DELAY*10);
}

/* end for imx6ull */


TouchCoreData *GetCoreData(void)
{
    return g_coreData;
}

static void TouchWakeupPoll(void)
{
    TouchCoreData *cd = GetCoreData();
    cd->readFinishFlag = true;
    wake_up_interruptible(&cd->pollWait);
}

uint32_t IrqHandle(uint32_t irqId, void *dev)
{
    (void)dev;
    int ret = gt9xx_irq_disable(irqId);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: disable irq failed, ret %d", __func__, ret);
    }
    (void)LOS_EventWrite(&g_touchEventIrq, EVENT_SYNC);

    return HDF_SUCCESS;
}

static void TouchConfigInit(TouchCoreData *cd)
{
    /* init waitqueue for poll */
    __init_waitqueue_head(&cd->pollWait);

    /* config i2c and input */
    cd->i2cClient.i2cCfg.addr = DRIVER_CHIP_I2C_ADDR;
    cd->i2cClient.i2cCfg.busNum = I2C_BUS_NUM;
    cd->inputCfg.solutionX = TOUCH_SOLUTION_X;
    cd->inputCfg.solutionY = TOUCH_SOLUTION_Y;

    /* config device type and module info */
    cd->inputDevType = INDEV_TYPE_TOUCH;
    (void)strncpy_s(cd->chipInfo, CHIP_INFO_LEN, "HZ1145130", strlen("HZ1145130"));
    (void)strncpy_s(cd->vendorName, VENDOR_NAME_LEN, "ZG1695", strlen("TG1695"));
    (void)strncpy_s(cd->chipName, CHIP_NAME_LEN, "GT9xx", strlen("GT9xx"));

    /* pin num and irq trigger config info */
    //cd->rstGpioNum = RST_GPIO_GROUP * GPIO_GROUP_SIZE + RST_GPIO_OFFSET;
    cd->intGpioNum = GT9XX_INT_NUM;
    cd->irqFlag = OSAL_IRQF_TRIGGER_FALLING;
    cd->shouldStop = false;
}


static int TouchSetupGpio(const TouchCoreData *cd)
{
	gt9xx_init();
    return HDF_SUCCESS;
}

static void ReadChipVersion(TouchCoreData *cd)
{
    unsigned char buf[GT_CFG_INFO_LEN] = {0};
    int ret = InputI2cRead(&cd->i2cClient, GT_CFG_INFO_ADDR, GT_ADDR_LEN, buf, GT_CFG_INFO_LEN);
    if (ret < 0) {
        HDF_LOGE("%s: read chip version failed", __func__);
        return;
    }

    int version = (buf[GT_FW_VER_HIGH] << ONE_BYTE_OFFSET) | buf[GT_FW_VER_LOW];
    int xSolution = (buf[GT_SOLU_X_HIGH] << ONE_BYTE_OFFSET) | buf[GT_SOLU_X_LOW];
    int ySolution = (buf[GT_SOLU_Y_HIGH] << ONE_BYTE_OFFSET) | buf[GT_SOLU_Y_LOW];
    HDF_LOGI("%s: IC FW version is 0x%x", __func__, version);
    if (buf[GT_FW_VER_HIGH] == 0x0) {
        HDF_LOGI("Product ID : %c%c%c_%02x%02x, X_Solu = %d, Y_Solu = %d", buf[GT_PROD_ID_1ST], buf[GT_PROD_ID_2ND],
            buf[GT_PROD_ID_3RD], buf[GT_FW_VER_HIGH], buf[GT_FW_VER_LOW], xSolution, ySolution);
    } else {
        HDF_LOGI("Product_ID: %c%c%c%c_%02x%02x, X_Solu = %d, Y_Solu = %d", buf[GT_PROD_ID_1ST], buf[GT_PROD_ID_2ND],
            buf[GT_PROD_ID_3RD], buf[GT_PROD_ID_4TH], buf[GT_FW_VER_HIGH], buf[GT_FW_VER_LOW], xSolution, ySolution);
    }
}

static int TouchSetupI2c(TouchCoreData *cd)
{
    /* get i2c handle */
    cd->i2cClient.i2cHandle = I2cOpen(cd->i2cClient.i2cCfg.busNum);
    if (cd->i2cClient.i2cHandle == NULL) {
        HDF_LOGE("%s: open i2c failed", __func__);
        return HDF_FAILURE;
    }

    ReadChipVersion(cd);
    return HDF_SUCCESS;
}

static int TouchCleanBuffer(void)
{
    TouchCoreData *cd = GetCoreData();
    unsigned char writeBuf[GT_CLEAN_DATA_LEN];
    writeBuf[GT_REG_HIGH_POS] = (GT_BUF_STATE_ADDR >> ONE_BYTE_OFFSET) & ONE_BYTE_MASK;
    writeBuf[GT_REG_LOW_POS] = GT_BUF_STATE_ADDR & ONE_BYTE_MASK;
    writeBuf[GT_CLEAN_POS] = GT_CLEAN_FLAG;
    int ret = InputI2cWrite(&cd->i2cClient, writeBuf, GT_CLEAN_DATA_LEN);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: InputI2cWrite fail, ret = %d", __func__, ret);
    }
    return ret;
}

static int EventHandler(const TouchCoreData *cd, InputEventData *event)
{
	static int bPressed = 0;
    unsigned char fingerNum = 0;
    int pointNum;
    (void)memset_s(event, sizeof(InputEventData), 0, sizeof(InputEventData));

    int ret = InputI2cRead(&cd->i2cClient, GT_BUF_STATE_ADDR, GT_ADDR_LEN, &fingerNum, 1);
    if (ret < 0) {
        return HDF_FAILURE;
    }

	//PRINT_RELEASE("%s %s %d, fingerNum= 0x%x\n", __FILE__, __FUNCTION__, __LINE__, fingerNum);
	
    if (fingerNum == GT_EVENT_INVALID) {
		return HDF_FAILURE;
		
    }
    if (fingerNum == GT_EVENT_UP) {
        g_touchEventData.pointNum = 0;
        g_touchEventData.definedEvent = TOUCH_EVENT_UP;

		if (bPressed) {
			bPressed = 0;
			//HDF_LOGE("%s: pointNum is invalid, %d", __func__, pointNum);
			event->pressure = 0;
			event->pointNum = 0;
			event->definedEvent = TOUCH_EVENT_DOWN;
			
			if (memcpy_s(&g_touchEventData, sizeof(InputEventData), event, sizeof(InputEventData)) != EOK) {
				HDF_LOGE("%s: memcpy_s fail", __func__);
				return HDF_FAILURE;
			}
		}	
        goto exit;
    }

    pointNum = fingerNum & GT_FINGER_NUM_MASK;
    if (pointNum > 0) {
        unsigned char eventBuf[GT_EVENT_SIZE] = {0};
        (void)InputI2cRead(&cd->i2cClient, GT_X_LOW_BYTE_BASE, GT_ADDR_LEN, eventBuf, GT_EVENT_SIZE);

		bPressed = 1;
		
        /* parse the i2c data */
        event->x = (eventBuf[GT_X_LOW] & ONE_BYTE_MASK) | ((eventBuf[GT_X_HIGH] & ONE_BYTE_MASK) << ONE_BYTE_OFFSET);
        event->y = (eventBuf[GT_Y_LOW] & ONE_BYTE_MASK) | ((eventBuf[GT_Y_HIGH] & ONE_BYTE_MASK) << ONE_BYTE_OFFSET);
		event->pressure = (eventBuf[GT_PRESSURE_LOW] & ONE_BYTE_MASK) | ((eventBuf[GT_PRESSURE_HIGH] & ONE_BYTE_MASK) << ONE_BYTE_OFFSET);
        event->pointNum = pointNum;
        event->definedEvent = TOUCH_EVENT_DOWN;
	
        if (memcpy_s(&g_touchEventData, sizeof(InputEventData), event, sizeof(InputEventData)) != EOK) {
            HDF_LOGE("%s: memcpy_s fail", __func__);
            return HDF_FAILURE;
        }
    } else {
		return HDF_FAILURE;
    }
exit:
    if (TouchCleanBuffer() != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static void TouchHandleEvent(void)
{
    InputEventData event;
    TouchCoreData *cd = GetCoreData();
    (void)memset_s(&event, sizeof(InputEventData), 0, sizeof(InputEventData));

    while (true) {
        int ret = LOS_EventRead(&g_touchEventIrq, EVENT_SYNC, LOS_WAITMODE_AND | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
        if (ret != EVENT_SYNC) {
            OsalMSleep(TASK_SLEEP_MS);
        } else {
            if (EventHandler(cd, &event) == HDF_SUCCESS) {
                TouchWakeupPoll();
            }
        }

		gt9xx_irq_clear(cd->intGpioNum);
        ret = gt9xx_irq_enable(cd->intGpioNum);
        if (ret != HDF_SUCCESS) {
            HDF_LOGE("%s: enable irq failed, ret %d", __func__, ret);
        }

        if (cd->shouldStop) {
            HDF_LOGE("%s: the event task should be stoped", __func__);
            break;
        }
    }
}

static int TouchIrqTaskInit(TouchCoreData *cd)
{
    /* init event for irq */
    int ret = LOS_EventInit(&g_touchEventIrq);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: LOS_EventInit failed, ret %d", __func__, ret);
        return HDF_FAILURE;
    }

    /* register irq */
	gt9xx_irq_init(cd);

    /* enable irq */
    ret = gt9xx_irq_enable(cd->intGpioNum);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: enable irq failed, ret %d", __func__, ret);
        return HDF_FAILURE;
    }

    /* set irq task which to handle point event data */
    TSK_INIT_PARAM_S handleEventTask = {0};
    UINT32 handleEventTaskID;
    handleEventTask.pfnTaskEntry = (TSK_ENTRY_FUNC)TouchHandleEvent;
    handleEventTask.uwStackSize  = TASK_SIZE;
    handleEventTask.pcName       = "HdfTouchEventHandler";
    handleEventTask.usTaskPrio   = TASK_PRIO_LEVEL_TWO;
    handleEventTask.uwResved     = LOS_TASK_STATUS_DETACHED;
    ret = LOS_TaskCreate(&handleEventTaskID, &handleEventTask);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: create event handle task failed, ret %d", __func__, ret);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int IoctlReadInputEvent(unsigned long arg)
{
    InputEventData *eventData = (InputEventData *)(uintptr_t)arg;

    if (eventData == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    if (LOS_ArchCopyToUser(eventData, &g_touchEventData, sizeof(g_touchEventData)) != 0) {
        HDF_LOGE("%s:copy chipInfo failed", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

static int IoctlGetDeviceType(unsigned long arg)
{
    unsigned int *devType = (unsigned int *)(uintptr_t)arg;
    TouchCoreData *cd = GetCoreData();

    if (devType == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (LOS_ArchCopyToUser(devType, &cd->inputDevType, sizeof(cd->inputDevType)) != 0) {
        HDF_LOGE("%s:copy devType failed", __func__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: devType is %u", __func__, cd->inputDevType);
    return HDF_SUCCESS;
}

static int IoctlGetChipInfo(unsigned long arg)
{
    TouchCoreData *cd = GetCoreData();
    char *chipInfo = (char *)(uintptr_t)arg;

    if (chipInfo == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    if (LOS_ArchCopyToUser(chipInfo, cd->chipInfo, CHIP_INFO_LEN) != 0) {
        HDF_LOGE("%s:copy chipInfo failed", __func__);
        return HDF_FAILURE;
    }

    HDF_LOGI("%s: chipInfo is %s", __func__, cd->chipInfo);
    return HDF_SUCCESS;
}

static int TouchIoctl(FAR struct file *filep, int cmd, unsigned long arg)
{
    int ret;
    if (filep == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }

    switch (cmd) {
        case INPUT_IOCTL_GET_EVENT_DATA:
            ret = IoctlReadInputEvent(arg);
            break;
        case INPUT_IOCTL_GET_DEVICE_TYPE:
            ret = IoctlGetDeviceType(arg);
            break;
        case INPUT_IOCTL_GET_CHIP_INFO:
            ret = IoctlGetChipInfo(arg);
            break;
        default:
            ret = 0;
            HDF_LOGE("%s: cmd unknown, cmd = 0x%x", __func__, cmd);
            break;
    }
    return ret;
}

#ifndef CONFIG_DISABLE_POLL
static int TouchPoll(FAR struct file *filep, poll_table *wait)
{
    unsigned int pollMask = 0;
    TouchCoreData *cd = GetCoreData();

    if (filep == NULL || wait == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    poll_wait(filep, &cd->pollWait, wait);
    if (cd->readFinishFlag == true) {
        pollMask |= POLLIN;
    }
    cd->readFinishFlag = false;
    return pollMask;
}
#endif

static int TouchOpen(FAR struct file *filep)
{
    HDF_LOGI("%s: called", __func__);
    if (filep == NULL) {
        HDF_LOGE("%s: fliep is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    return HDF_SUCCESS;
}

static int TouchClose(FAR struct file *filep)
{
    HDF_LOGI("%s: called", __func__);
    TouchCoreData *cd = GetCoreData();

    if (filep == NULL) {
        HDF_LOGE("%s: fliep is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    __init_waitqueue_head(&cd->pollWait);
    return HDF_SUCCESS;
}

static const struct file_operations_vfs g_touchDevOps = {
    .open = TouchOpen,
    .close = TouchClose,
    .read = NULL,
    .write = NULL,
    .seek = NULL,
    .ioctl = TouchIoctl,
    .mmap = NULL,
#ifndef CONFIG_DISABLE_POLL
    .poll = TouchPoll,
#endif
    .unlink = NULL,
};


int32_t Gt911Dispatch(struct HdfDeviceIoClient *client, int cmdId, struct HdfSBuf *data, struct HdfSBuf *reply)
{
    (void)client;
    (void)cmdId;
    if (data == NULL || reply == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t Gt9xxTouchDriverOpen(struct HdfDeviceObject *object)
{
    if (object == NULL) {
        HDF_LOGE("%s: param is null", __func__);
        return HDF_ERR_INVALID_PARAM;
    }
    static struct IDeviceIoService service = {
        .object = {0},
        .Dispatch = Gt911Dispatch,
    };
    object->service = &service;
    return HDF_SUCCESS;
}

int Gt9xxTouchDriverInit(struct HdfDeviceObject *object)
{
    (void)object;
    HDF_LOGI("%s: enter", __func__);
    g_coreData = (TouchCoreData *)OsalMemAlloc(sizeof(TouchCoreData));
    if (g_coreData == NULL) {
        HDF_LOGE("%s: malloc failed", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }
    (void)memset_s(g_coreData, sizeof(TouchCoreData), 0, sizeof(TouchCoreData));
    TouchConfigInit(g_coreData);

    if (TouchSetupGpio(g_coreData)) {
        goto ERR_EXIT;
    }

    if (TouchSetupI2c(g_coreData)) {
        goto ERR_EXIT;
    }

    if (TouchIrqTaskInit(g_coreData)) {
        goto ERR_EXIT;
    }

    (void)mkdir("/dev/input", DEFAULT_DIR_MODE);
    int ret = register_driver(TOUCH_DEVICE, &g_touchDevOps, TOUCH_DEVICE_MODE, NULL);
    if (ret != 0) {
        HDF_LOGE("%s: register touch dev failed, ret %d", __func__, ret);
        goto ERR_EXIT;
    }
    HDF_LOGI("%s: exit succ", __func__);
    return HDF_SUCCESS;

ERR_EXIT:
    if (g_coreData->i2cClient.i2cHandle != NULL) {
        I2cClose(g_coreData->i2cClient.i2cHandle);
        g_coreData->i2cClient.i2cHandle = NULL;
    }
    OsalMemFree(g_coreData);
    g_coreData = NULL;
    return HDF_FAILURE;
}

struct HdfDriverEntry g_gt9xxTouchDevEntry = {
    .moduleVersion = 1,
    .moduleName = "HDF_TOUCHSCREEN",
    .Bind = Gt9xxTouchDriverOpen,
    .Init = Gt9xxTouchDriverInit,
};

HDF_INIT(g_gt9xxTouchDevEntry);
