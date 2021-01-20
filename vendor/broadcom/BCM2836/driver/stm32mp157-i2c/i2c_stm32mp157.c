/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: hi35xx i2c driver implement.
 * Author: yanghaizhou
 * Create: 2020-07-25
 */

#include "asm/platform.h"
#include "device_resource_if.h"
#include "hdf_device_desc.h"
#include "hdf_log.h"
#include "i2c_core.h"
#include "i2c_dev.h"
#include "los_hwi.h"
#include "osal_io.h"
#include "osal_mem.h"
#include "osal_time.h"

/* 
 *IC2状态码，方便通过返回值对程序返回处进行定位
 */
#define I2C_OK				     (0)
#define I2C_ERROR                (1)
#define I2C_BUSY				 (2)
#define I2C_IDLE				 (3)
#define I2C_NAK				     (4)
#define I2C_ARBITRATIONLOST	     (5)
#define I2C_TIMEOUT			     (6)
#define I2C_ADDRNAK			     (7)


/* 寄存器地址的宏结构体定义，此种方式仅定义入口地址即可 */
/* all registers address is Base address + xh offset*/
typedef struct tagRegisters{
  volatile uint16_t IADR;                              /*I2C Address Register, offset: 0x0 */
           uint8_t ReservedIADR[2];
  volatile uint16_t IFDR;                              /*I2C Frequency Divider Register, offset: 0x4 */
           uint8_t ReservedIFDR[2];
  volatile uint16_t I2CR;                              /*I2C Control Register, offset: 0x8 */
           uint8_t ReservedI2CR[2];
  volatile uint16_t I2SR;                              /*I2C Status Register, offset: 0xC */
           uint8_t ReservedI2SR[2];
  volatile uint16_t I2DR;                              /*I2C Data I/O Register, offset: 0x10 */
} I2C_REGISTERS;

/*
 * IC2操作码定义
 */
typedef enum enI2C_OPCODE
{
    I2C_WRITE = 0,            /* 主机向从机写数据 */
    I2C_READ  = 1,  		/* 主机从从机读数据 */
    I2C_DONOTHING_BULL
} I2C_OPCODE;

/*
 * 主机传输结构体
 */
typedef struct tagI2cTransfer
{
    uint8_t  ucSlaveAddress;      	     /* 7位从机地址 */
    uint32_t ulOpcode  ; 		     /* 操作码*/
    uint32_t ulSubAddress;       		/* 目标寄存器地址 */
    uint8_t  ulSubAddressLen;    	     /* 寄存器地址长度 */
    volatile uint32_t ulLenth;  	     /* 数据长度 */
    uint8_t *volatile pbuf;    	     /* 数据*/
} I2C_TRANSFER;


#define HDF_LOG_TAG i2c_imx6ull

#define USER_VFS_SUPPORT


struct Imx6ullI2cCntlr {
    struct I2cCntlr cntlr;
    volatile unsigned char  *regBase;
    int16_t regSize;
    int16_t bus;
    uint32_t clk;
    uint32_t freq;
    uint32_t irq;
    uint32_t regBasePhy;
};

struct Imx6ullTransferData {
    struct I2cMsg *msgs;
    int16_t index;
    int16_t count;
};

/*! @name SW_MUX_CTL_PAD - SW_MUX_CTL_PAD_JTAG_MOD SW MUX Control Register..SW_MUX_CTL_PAD_CSI_DATA07 SW MUX Control Register */
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_MASK      (0xFU)  /* Merged from fields with different position or width, of widths (3, 4), largest definition used */
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_SHIFT     (0U)
#define IOMUXC_SW_MUX_CTL_PAD_MUX_MODE(x)        (((uint32_t)(((uint32_t)(x)) << IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_SHIFT)) & IOMUXC_SW_MUX_CTL_PAD_MUX_MODE_MASK)  /* Merged from fields with different position or width, of widths (3, 4), largest definition used */
#define IOMUXC_SW_MUX_CTL_PAD_SION_MASK          (0x10U)
#define IOMUXC_SW_MUX_CTL_PAD_SION_SHIFT         (4U)
#define IOMUXC_SW_MUX_CTL_PAD_SION(x)            (((uint32_t)(((uint32_t)(x)) << IOMUXC_SW_MUX_CTL_PAD_SION_SHIFT)) & IOMUXC_SW_MUX_CTL_PAD_SION_MASK)
/*! @name SELECT_INPUT - USB_OTG1_ID_SELECT_INPUT DAISY Register..USDHC2_WP_SELECT_INPUT DAISY Register */
#define IOMUXC_SELECT_INPUT_DAISY_MASK           (0x7U)  /* Merged from fields with different position or width, of widths (1, 2, 3), largest definition used */
#define IOMUXC_SELECT_INPUT_DAISY_SHIFT          (0U)
#define IOMUXC_SELECT_INPUT_DAISY(x)             (((uint32_t)(((uint32_t)(x)) << IOMUXC_SELECT_INPUT_DAISY_SHIFT)) & IOMUXC_SELECT_INPUT_DAISY_MASK)  /* Merged from fields with different position or width, of widths (1, 2, 3), largest definition used */

#define IOMUXC_UART4_TX_DATA_I2C1_SCL                        IO_DEVICE_ADDR(0x020E00B4), 0x2U, IO_DEVICE_ADDR(0x020E05A4), 0x1U, IO_DEVICE_ADDR(0x020E0340)
#define IOMUXC_UART4_RX_DATA_I2C1_SDA                        IO_DEVICE_ADDR(0x020E00B8), 0x2U, IO_DEVICE_ADDR(0x020E05A8), 0x2U, IO_DEVICE_ADDR(0x020E0344)

#define IOMUXC_UART5_TX_DATA_I2C2_SCL                        IO_DEVICE_ADDR(0x020E00BCU), 0x2U, IO_DEVICE_ADDR(0x020E05ACU), 0x2U, IO_DEVICE_ADDR(0x020E0348U)
#define IOMUXC_UART5_RX_DATA_I2C2_SDA                        IO_DEVICE_ADDR(0x020E00C0U), 0x2U, IO_DEVICE_ADDR(0x020E05B0U), 0x2U, IO_DEVICE_ADDR(0x020E034CU)

static inline void IOMUXC_SetPinMux(uint32_t muxRegister,
                                    uint32_t muxMode,
                                    uint32_t inputRegister,
                                    uint32_t inputDaisy,
                                    uint32_t configRegister,
                                    uint32_t inputOnfield)
{
	(void)configRegister;
	
    *((volatile uint32_t *)muxRegister) =
        IOMUXC_SW_MUX_CTL_PAD_MUX_MODE(muxMode) | IOMUXC_SW_MUX_CTL_PAD_SION(inputOnfield);

    if (inputRegister)
    {
        *((volatile uint32_t *)inputRegister) = IOMUXC_SELECT_INPUT_DAISY(inputDaisy);
    }
}

static inline void IOMUXC_SetPinConfig(uint32_t muxRegister,
									   uint32_t muxMode,
									   uint32_t inputRegister,
									   uint32_t inputDaisy,
									   uint32_t configRegister,
									   uint32_t configValue)
{
	(void)muxRegister;
	(void)muxMode;
	(void)inputRegister;
	(void)inputDaisy;
	
	if (configRegister)
	{
		*((volatile uint32_t *)configRegister) = configValue;
	}
}

static void i2c_gpio_init(int bus)
{
	if (bus == 0)
	{
		/*初始化I2C2*/
		IOMUXC_SetPinMux(IOMUXC_UART5_TX_DATA_I2C2_SCL, 1);
		IOMUXC_SetPinMux(IOMUXC_UART5_RX_DATA_I2C2_SDA, 1);
		
		
		IOMUXC_SetPinConfig(IOMUXC_UART5_TX_DATA_I2C2_SCL, 0x70B0);
		IOMUXC_SetPinConfig(IOMUXC_UART5_RX_DATA_I2C2_SDA, 0X70B0);
	}
	else
	{
		/* no implementations */
		/*I2C1 复用UART4	SCL-TXD SDA-RXD*/
		IOMUXC_SetPinMux(IOMUXC_UART4_TX_DATA_I2C1_SCL, 1);
		IOMUXC_SetPinMux(IOMUXC_UART4_RX_DATA_I2C1_SDA, 1);
		IOMUXC_SetPinConfig(IOMUXC_UART4_TX_DATA_I2C1_SCL, 0x70B0);
		IOMUXC_SetPinConfig(IOMUXC_UART4_RX_DATA_I2C1_SDA, 0X70B0);
	}
}

static void i2c_init(I2C_REGISTERS *I2C_BASE)
{
    /*I2C_I2CR是控制寄存器,
     * 可以: 使能I2C,使能中断, 选择主从模式.
     */

    /* 配置I2C控制器步骤: 关闭I2C,配置,打开I2C */

    /* 设置SCL时钟为100K
     * I2C的时钟源来源于IPG_CLK_ROOT=49.5Mhz
	 *	PLL2 = 528 MHz
	 *	PLL2_PFD2 = 528 *18 /24 = 396 MHz
	 *	IPG_CLK_ROOT = (PLL2_PFD2 / ahb_podf )/ ipg_podf = (396 MHz/4)/2 = 49.5Mhz
	 *	
	 *	PER_CLK_ROOT = IPG_CLK_ROOT/perclk_podf = 49.5 MHz/1 = 49.5 MHz
	 * 设置I2C的波特率为100K， 因此当分频值=49500000/100000=495	
	 * 参考Table 31-3. I2C_IFDR Register Field Values 表中0x37对应的512最接近
	 * 即寄存器IFDR的IC位设置为0X37
	 */	 
	I2C_BASE->I2CR &= ~(1 << 7);
	I2C_BASE->IFDR = 0x37;
	I2C_BASE->I2CR |= (1<<7);
}

static uint8_t i2c_check(I2C_REGISTERS *I2C_BASE, uint32_t status)
{
	/* 检查是否发生仲裁丢失错误(arbitration lost) */
	if(status & (1<<4))
	{
		I2C_BASE->I2SR &= ~(1<<4);	/* 清除仲裁丢失错误位 			*/

		I2C_BASE->I2CR &= ~(1 << 7);	/* 复位I2C: 先关闭I2C 				*/
		I2C_BASE->I2CR |= (1 << 7);	/* 再打开I2C 				*/
		return I2C_ARBITRATIONLOST;
	} 
	else if(status & (1 << 0))     	/* 检查NAK */
	{
		return I2C_NAK;		/* 返回NAK(无应答) */
	}
	return I2C_OK;

}

static uint8_t i2c_start(I2C_REGISTERS *I2C_BASE, uint8_t ucSlaveAddr, uint32_t ulOpcode)
{

	if(I2C_BASE->I2SR & (1 << 5))			/* I2C忙 */
		return 1;

	/*
         * 设置控制寄存器I2CR
         * bit[5]: 1 主模式(master)
         * bit[4]: 1 发送(transmit)
	 */
	I2C_BASE->I2CR |=  (1 << 5) | (1 << 4);

	/*
         * 设置数据寄存器I2DR
         * bit[7:0] : 要发送的数据, 
         * START信号后第一个数据是从设备地址
	 */ 
	I2C_BASE->I2DR = ((uint32_t)ucSlaveAddr << 1) | ((I2C_READ == ulOpcode)? 1 : 0);
	return 0;

}

static uint8_t i2c_stop(I2C_REGISTERS *I2C_BASE)
{

	uint16_t usTimeout = 0xffff;

	/*
	 * 清除控制寄存器I2CR[5:3]
         * 发出STOP信号
	 */
	I2C_BASE->I2CR &= ~((1 << 5) | (1 << 4) | (1 << 3));

	/* 等待STOP信号确实发出去了 */
	while((I2C_BASE->I2SR & (1 << 5)))
	{
		usTimeout--;
		if(usTimeout == 0)	/* 超时跳出 */
			return I2C_TIMEOUT;
	}
	return I2C_OK;

}

static uint8_t i2c_restart(I2C_REGISTERS *I2C_BASE, uint8_t ucSlaveAddr, uint32_t ulOpcode)
{

	/* I2C忙并且工作在从模式,跳出 */
	if(I2C_BASE->I2SR & (1 << 5) && (((I2C_BASE->I2CR) & (1 << 5)) == 0))		
		return 6;

	/*
         * 设置控制寄存器I2CR
         * bit[4]: 1 发送(transmit)
         * bit[2]: 1 产生重新开始信号(Repeat start)
	 */
	I2C_BASE->I2CR |=  (1 << 4) | (1 << 2);

	/*
         * 设置数据寄存器I2DR
         * bit[7:0] : 要发送的数据, 
         * START信号后第一个数据是从设备地址
	 */ 
	I2C_BASE->I2DR = ((uint32_t)ucSlaveAddr << 1) | ((I2C_READ == ulOpcode)? 1 : 0);
	
	return 0;

}


static void i2c_write(I2C_REGISTERS *I2C_BASE, const uint8_t *pbuf, uint32_t len)
{
	/* 等待数据寄存器就绪,可以再次发送数据 */
	while(!(I2C_BASE->I2SR & (1 << 7))); 
	
	I2C_BASE->I2SR &= ~(1 << 1); 	  /* 清除IICIF */
	I2C_BASE->I2CR |= 1 << 4;	      /* 发送数据(transmit) */
	while(len--)
	{
		I2C_BASE->I2DR = *pbuf++; 	    /* 将buf中的数据写入到数据寄存器I2DR */
		
		while(!(I2C_BASE->I2SR & (1 << 1)));  /* 等待传输完成,完成或失败,中断状态位被置1 */	
		I2C_BASE->I2SR &= ~(1 << 1);			/* 清除中断状态位 */

		/* 检查有无错误 */
		if(i2c_check(I2C_BASE, I2C_BASE->I2SR))
			break;
	}
	
	I2C_BASE->I2SR &= ~(1 << 1);     /* 清除中断状态位 */
	i2c_stop(I2C_BASE); 	         /* 发送停止信号 */

}

static void i2c_read(I2C_REGISTERS *I2C_BASE, uint8_t *pbuf, uint32_t len)
{
	volatile uint8_t dummy = 0;
	dummy++; 	/* 防止编译警告 */

	/* 等待数据寄存器就绪 */
	while(!(I2C_BASE->I2SR & (1 << 7))); 
	
	I2C_BASE->I2SR &= ~(1 << 1); 			   /* 清除IICIF */
	I2C_BASE->I2CR &= ~((1 << 4) | (1 << 3));	/* 接收数据: Receive,TXAK */
	
	/* 如果只接收一个字节数据的话发送NACK信号 */
	if(len == 1)
        I2C_BASE->I2CR |= (1 << 3);

	dummy = I2C_BASE->I2DR; /* 假读 */


	while(len--)
	{
		while(!(I2C_BASE->I2SR & (1 << 1))); 	/* 等待传输完成 */	
		I2C_BASE->I2SR &= ~(1 << 1);			/* 清除标志位 */

	 	if(len == 0)
        {
        	i2c_stop(I2C_BASE); 			/* 发送停止信号 */
        }

        if(len == 1)
        {
            I2C_BASE->I2CR |= (1 << 3);
        }
		*pbuf++ = I2C_BASE->I2DR;
	}

}

static uint8_t Imx6ullI2cXferOneMsgPolling(I2C_REGISTERS *I2C_BASE, I2C_TRANSFER *transfer)
{
	uint32_t ulRet = 0;
	uint32_t ulOpcode = transfer->ulOpcode;

	/*开始前准备工作，清除标志位
	 *bit-4 IAL 仲裁位，bit-1 IIF 中断标志位
	 */
	I2C_BASE->I2SR &= ~((1 << 1) | (1 << 4));
	/* 等待传输完成 */
	while(!((I2C_BASE->I2SR >> 7) & 0X1)){}; 

	/* 如果要读某个寄存器,寄存器地址要先"写"给从设备
	 * 所以方向要"先写","后读"
	 */
    if ((transfer->ulSubAddressLen > 0) && (transfer->ulOpcode == I2C_READ))
    {
        ulOpcode = I2C_WRITE;
    }
	ulRet = i2c_start(I2C_BASE, transfer->ucSlaveAddress, ulOpcode);

	if (ulRet)
	{
		return ulRet;
	}
	
	/* 等待传输完成: 中断状态为会被置1 */
	while(!(I2C_BASE->I2SR & (1 << 1))){};

	/* 检查是否出错 */
	ulRet = i2c_check(I2C_BASE, I2C_BASE->I2SR);

	if (ulRet)
	{
	    i2c_stop(I2C_BASE); 			/* 发送停止信号 */
		return ulRet;
	}

	/*如果ulSubAddressLen不为0，表示要发送寄存器地址*/
	if (transfer->ulSubAddressLen)
	{
		do
		{
			/* 清除中断状态位 */
		    I2C_BASE->I2SR &= ~(1 << 1); 
			
			/* 调整长度, 也许寄存器地址有多个字节, 本程序最多支持4字节 */
			transfer->ulSubAddressLen--;

			I2C_BASE->I2DR = ((transfer->ulSubAddress) >> (8 * transfer->ulSubAddressLen)); 
  
			while(!(I2C_BASE->I2SR & (1 << 1))){};  	/* 等待传输完成: 中断状态位被置1 */

            /* 检查是否出错 */
            ulRet = i2c_check(I2C_BASE, I2C_BASE->I2SR);
            if(ulRet)
            {
             	i2c_stop(I2C_BASE); 				/* 出错:发送停止信号 */
             	return ulRet;
            }
		}
		while ((transfer->ulSubAddressLen > 0) && (ulRet == I2C_OK));

		if (I2C_READ == transfer->ulOpcode)
		{
            I2C_BASE->I2SR &= ~(1 << 1);			/* 清除中断状态位 */
            i2c_restart(I2C_BASE, transfer->ucSlaveAddress, I2C_READ); /* 发送重复开始信号和从机地址 */
    		while(!(I2C_BASE->I2SR & (1 << 1))){}; /* 等待传输完成: 中断状态位被置1 */

            /* 检查是否出错 */
			ulRet = i2c_check(I2C_BASE, I2C_BASE->I2SR);
			
            if(ulRet)
            {
             	ulRet = I2C_ADDRNAK;
                i2c_stop(I2C_BASE); 		/* 出错:发送停止信号 */
                return ulRet;  
            }
           	       

		}
		
	}
    /* 发送数据 */
    if ((I2C_WRITE == transfer->ulOpcode) && (transfer->ulLenth > 0))
    {
    	i2c_write(I2C_BASE, transfer->pbuf, transfer->ulLenth);
	}

    /* 读取数据 */
    if ((I2C_READ == transfer->ulOpcode) && (transfer->ulLenth > 0))
    {
       	i2c_read(I2C_BASE, transfer->pbuf, transfer->ulLenth);
	}
	return 0;	

}


static int32_t Imx6ullI2cTransfer(struct I2cCntlr *cntlr, struct I2cMsg *msgs, int16_t count)
{
    int32_t ret = HDF_SUCCESS;
    unsigned long irqSave;
    struct Imx6ullI2cCntlr *imx6ullI2Ccnt = NULL;
	I2C_TRANSFER transfer;
	int i;

    if (cntlr == NULL || cntlr->priv == NULL) {
        HDF_LOGE("Hi35xxI2cTransfer: cntlr lor imx6ullI2Ccntis null!\n");
        return HDF_ERR_INVALID_OBJECT;
    }
    imx6ullI2Ccnt = (struct Imx6ullI2cCntlr *)cntlr;

    if (msgs == NULL || count <= 0) {
        HDF_LOGE("Hi35xxI2cTransfer: err parms! count:%d\n", count);
        return HDF_ERR_INVALID_PARAM;
    }

    irqSave = LOS_IntLock();
    for (i = 0; i < count; i++) {
		transfer.ucSlaveAddress = msgs[i].addr;
		transfer.ulOpcode = (msgs[i].flags & I2C_FLAG_READ) ? I2C_READ : I2C_WRITE;
		transfer.ulSubAddress = 0;
		transfer.ulSubAddressLen = 0;
		transfer.ulLenth = msgs[i].len;
		transfer.pbuf    = msgs[i].buf;
	
        ret = Imx6ullI2cXferOneMsgPolling((I2C_REGISTERS *)imx6ullI2Ccnt->regBase, &transfer);
        if (ret != 0) {
            break;
        }
    }
    LOS_IntRestore(irqSave);
    return (i > 0) ? i : ret;
}

static struct I2cMethod g_method = {
    .transfer = Imx6ullI2cTransfer,
};

static int32_t Imx6ullI2cReadDrs(struct Imx6ullI2cCntlr *imx6ullI2Ccnt, const struct DeviceResourceNode *node)
{
    int32_t ret;
    uint32_t tmp;
    struct DeviceResourceIface *drsOps = NULL;

    drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetUint32 == NULL) {
        HDF_LOGE("%s: invalid drs ops fail!\n", __func__);
        return HDF_FAILURE;
    }

    ret = drsOps->GetUint32(node, "reg_pbase", &imx6ullI2Ccnt->regBasePhy, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read regBase fail!\n", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "reg_size", &tmp, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read regsize fail!\n", __func__);
        return ret;
    }
    imx6ullI2Ccnt->regSize = (uint16_t)tmp;

    ret = drsOps->GetUint32(node, "freq", &imx6ullI2Ccnt->freq, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read freq fail!\n", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "irq", &imx6ullI2Ccnt->irq, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read irq fail! \n", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "clk", &imx6ullI2Ccnt->clk, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read clk fail!\n", __func__);
        return ret;
    }

    ret = drsOps->GetUint32(node, "bus", &tmp, 0);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read bus fail!\n", __func__);
        return ret;
    }
    imx6ullI2Ccnt->bus = (uint16_t)tmp;

    return HDF_SUCCESS;
}

static int32_t Imx6ullI2cBind(struct HdfDeviceObject *device)
{
    int32_t ret;
    struct Imx6ullI2cCntlr *imx6ullI2Ccnt = NULL;

    HDF_LOGI("%s: Enter", __func__);
    if (device == NULL || device->property == NULL) {
        HDF_LOGE("%s: device or property is NULL\n", __func__);
        return HDF_ERR_INVALID_OBJECT;
    }

    imx6ullI2Ccnt = (struct Imx6ullI2cCntlr *)OsalMemCalloc(sizeof(*imx6ullI2Ccnt));
    if (imx6ullI2Ccnt == NULL) {
        HDF_LOGE("%s: malloc imx6ullI2Ccnt fail!", __func__);
        return HDF_ERR_MALLOC_FAIL;
    }

    ret = Imx6ullI2cReadDrs(imx6ullI2Ccnt, device->property);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: read drs fail! ret:%d", __func__, ret);
        goto __ERR__;
    }

    imx6ullI2Ccnt->regBase = OsalIoRemap(imx6ullI2Ccnt->regBasePhy, imx6ullI2Ccnt->regSize);
    if (imx6ullI2Ccnt->regBase == NULL) {
        HDF_LOGE("%s: ioremap regBase fail!\n", __func__);
        ret = HDF_ERR_IO;
        goto __ERR__;
    }

	i2c_gpio_init(imx6ullI2Ccnt->bus);
    i2c_init((I2C_REGISTERS *)imx6ullI2Ccnt->regBase);

    imx6ullI2Ccnt->cntlr.device = device;
    imx6ullI2Ccnt->cntlr.priv = (void *)device->property;
    imx6ullI2Ccnt->cntlr.busId = imx6ullI2Ccnt->bus;
    imx6ullI2Ccnt->cntlr.ops = &g_method;
    ret = I2cCntlrAdd(&imx6ullI2Ccnt->cntlr);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("%s: add i2c controller fail:%d!\n", __func__, ret);
        goto __ERR__;
    }

#ifdef USER_VFS_SUPPORT
    (void)I2cAddVfsById(imx6ullI2Ccnt->cntlr.busId);
#endif
    return HDF_SUCCESS;
__ERR__:
    if (imx6ullI2Ccnt != NULL) {
        if (imx6ullI2Ccnt->regBase != NULL) {
            OsalIoUnmap((void *)imx6ullI2Ccnt->regBase);
            imx6ullI2Ccnt->regBase = NULL;
        }
        OsalMemFree(imx6ullI2Ccnt);
        imx6ullI2Ccnt = NULL;
    }
    return ret;
}

static int32_t Imx6ullI2cInit(struct HdfDeviceObject *device)
{
    (void)device;
    return HDF_SUCCESS;
}

static void Imx6ullI2cRelease(struct HdfDeviceObject *device)
{
    struct I2cCntlr *cntlr = NULL;
    struct Imx6ullI2cCntlr *imx6ullI2Ccnt = NULL;

    HDF_LOGI("%s: enter\n", __func__);

    if (device == NULL) {
        HDF_LOGE("%s: device is null!\n", __func__);
        return;
    }

    cntlr = I2cCntlrFromDevice(device);
    if (cntlr == NULL) {
        HDF_LOGE("%s: no service binded!\n", __func__);
        return;
    }
    I2cCntlrRemove(cntlr);

    imx6ullI2Ccnt = (struct Imx6ullI2cCntlr *)cntlr;
    OsalIoUnmap((void *)imx6ullI2Ccnt->regBase);
    OsalMemFree(imx6ullI2Ccnt);
}

struct HdfDriverEntry g_i2cDriverEntry = {
    .moduleVersion = 1,
    .Bind = Imx6ullI2cBind,
    .Init = Imx6ullI2cInit,
    .Release = Imx6ullI2cRelease,
    .moduleName = "HDF_PLATFORM_I2C",
};
HDF_INIT(g_i2cDriverEntry);

