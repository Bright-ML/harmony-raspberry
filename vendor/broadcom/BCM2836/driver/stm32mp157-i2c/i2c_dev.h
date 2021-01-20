/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: hi35xx i2c driver user support.
 * Author: yanghaizhou
 * Create: 2020-08-11
 */

#ifndef I2C_USER_H
#define I2C_USER_H

#include "hdf_base.h"

enum I2cIoctlCmd {
    IOCTL_RETRIES      = 0x0701,
    IOCTL_TIMEOUT      = 0x0702,
    IOCTL_SLAVE        = 0x0703,
    IOCTL_SLAVE_FORCE  = 0x0706,
    IOCTL_TENBIT       = 0x0704,
    IOCTL_FUNCS        = 0x0705,
    IOCTL_RDWR         = 0x0707,
    IOCTL_PEC          = 0x0708,
    IOCTL_SMBUS        = 0x0720,
    IOCTL_16BIT_REG    = 0x0709,  /* 16BIT REG WIDTH */
    IOCTL_16BIT_DATA   = 0x070a,  /* 16BIT DATA WIDTH */
};

enum I2cMsgFlag {
    I2C_M_RD           = 0x0001,
    I2C_M_TEN          = 0x0010,
    I2C_M_RECV_LEN     = 0x0400,
    I2C_M_NO_RD_ACK    = 0x0800,
    I2C_M_IGNORE_NAK   = 0x1000,
    I2C_M_REV_DIR_ADDR = 0x2000,
    I2C_M_NOSTART      = 0x4000,
    I2C_M_STOP         = 0x8000,
#ifdef __LITEOS__
    I2C_M_16BIT_DATA   = 0x0008,
    I2C_M_16BIT_REG    = 0x0002,
#endif
};

typedef struct i2c_msg {
    uint16_t addr;
    uint16_t flags;
    uint16_t len;
    uint8_t *buf;
} I2cMsgUser;

typedef struct i2c_rdwr_ioctl_data {
    struct i2c_msg *msgs;
    unsigned int nmsgs;
} I2cIoctlWrap;

int32_t I2cAddVfsById(int16_t id);

void I2cRemoveVfsById(int16_t id);

#endif /* I2C_USER_H */
