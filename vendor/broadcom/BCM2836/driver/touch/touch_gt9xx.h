/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: ft6236 touch driver interface.
 * Author: zhaihaipeng
 * Create: 2020-07-25
 */

#ifndef TOUCH_GT911_H
#define TOUCH_GT911_H

#include <stdint.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "i2c_if.h"

/* screen resolution */
#define TOUCH_SOLUTION_X 1024
#define TOUCH_SOLUTION_Y 600
/* driver chip i2c address */
#define DRIVER_CHIP_I2C_ADDR 0x5D
#define I2C_BUS_NUM 0
/* pin num of reset and irq */
#define GPIO_GROUP_SIZE 8
#define RST_GPIO_GROUP 1
#define RST_GPIO_OFFSET 5
#define INT_GPIO_GROUP 1
#define INT_GPIO_OFFSET 4
/* the config of reset delay */
#define RESET_LOW_DELAY 10
#define RESET_HIGH_DELAY 60
/* IO config for int-pin and I2C-pin */
#define I2C6_DATA_REG_ADDR 0x114f004c
#define I2C6_CLK_REG_ADDR 0x114f0048
#define INR_REG_ADDR 0x10ff002C
#define RST_REG_ADDR 0x10ff0030
#define I2C_REG_CFG 0x403
#define INT_REG_CFG 0x400
#define RST_REG_CFG 0x400
/* length of string about device info */
#define CHIP_INFO_LEN 10
#define CHIP_NAME_LEN 10
#define VENDOR_NAME_LEN 10
#define SELF_TEST_RESULT_LEN 20

typedef enum {
    EVENT_DOWN,
    EVENT_UP,
    EVENT_CONTACT,
} EventType;

typedef enum {
    INPUT_SUCCESS,
    INPUT_EINVAL,
    INPUT_EFAULT,
    INPUT_ENOMEM,
    INPUT_ETIMEOUT,
    INPUT_EUNSUPPORT,
    INPUT_EUNKNOWN,
} RetStatus;

enum InputDevType {
    INDEV_TYPE_TOUCH,
    INDEV_TYPE_KEY,
    INDEV_TYPE_KEYBOARD,
    INDEV_TYPE_MOUSE,
    INDEV_TYPE_BUTTON,
    INDEV_TYPE_CROWN,
    INDEV_TYPE_ENCODER, /* Encoder with specific function or event */
    INDEV_TYPE_MAX,
};

enum PowerStatus {
    INPUT_RESUME = 0,
    INPUT_SUSPEND = 1,
    INPUT_LOW_POWER = 2,
    INPUT_POWER_STATUS_UNKNOWN,
};

/** multi touch info */
typedef struct {
    int x;    /* x coordinate */
    int y;    /* y coordinate */
    int pressure;
    int definedEvent;    /* touch event: 0-down; 1-up; 2-contact */
    int fingerID;        /* touch ID */
    int pointNum;
    struct timeval timeStamp;
    bool moreDataFlag;
} InputEventData;


typedef struct {
    unsigned int testType;
    char testResult[SELF_TEST_RESULT_LEN + 1];
} CapacitanceTestInfo;

typedef struct {
    const char *cmdCode;
    const char *cmdValue;
} InputExtraCmd;

struct InputConfig {
    unsigned int solutionX;
    unsigned int solutionY;
};

typedef struct {
    unsigned int inputDevType;
    wait_queue_head_t pollWait;
    bool readFinishFlag;
    bool shouldStop;
    struct InputConfig inputCfg;
    InputI2cClient i2cClient;
    int intGpioNum;
    int rstGpioNum;
    unsigned int irqFlag;
    unsigned int powerStatus;
    char chipInfo[CHIP_INFO_LEN];
    char vendorName[VENDOR_NAME_LEN];
    char chipName[CHIP_NAME_LEN];
} TouchCoreData;

enum TouchIoctlCmd {
    INPUT_IOCTL_GET_EVENT_DATA,
    INPUT_IOCTL_SET_POWER_STATUS,
    INPUT_IOCTL_GET_POWER_STATUS,
    INPUT_IOCTL_GET_DEVICE_TYPE,
    INPUT_IOCTL_GET_CHIP_INFO,
    INPUT_IOCTL_GET_VENDOR_NAME,
    INPUT_IOCTL_GET_CHIP_NAME,
    INPUT_IOCTL_SET_GESTURE_MODE,
    INPUT_IOCTL_RUN_CAPACITANCE_TEST,
    INPUT_IOCTL_RUN_EXTRA_CMD,
};

#endif
