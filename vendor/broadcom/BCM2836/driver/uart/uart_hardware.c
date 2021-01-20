#include "linux/delay.h"
#include "asm/io.h"

#include "los_typedef.h"
#include "los_task.h"
#include "los_base.h"
#include "los_event.h"
#include "errno.h"

#include "linux/interrupt.h"
#include "linux/kernel.h"
#include "linux/spinlock.h"
#include "asm/platform.h"
#include <sys/bus.h>

#include "uart.h"
#include "uart_dev.h"
#include "string.h"

#include "los_magickey.h"
#include "uart_hardware.h"

//EVENT_CB_S g_stShellEvent;

//CHAR g_inputCmd[CMD_LENGTH];
//INT32 g_inputIdx = 0;
__attribute__ ((section(".data"))) UINT32 g_uart_fputc_en = 1;


/*---------自定义函数----------*/
struct BCM2836_port {
        int enable;
        unsigned long phys_base;
        unsigned int irq_num;
        struct uart_driver_data *udd;
};


#define RPI_BASE_UART_REGISTER (0x3f201000) //HI3516:0x120A0000 rpi2b:0x3F201000
#define AMBA_UART_DR (*(volatile unsigned char *)(RPI_BASE_UART_REGISTER + 0x00))
#define AMBA_UART_FR (*(volatile unsigned char *)(RPI_BASE_UART_REGISTER + 0x18))

#define RPI_BASE_UART_REGISTER1 IO_DEVICE_ADDR(0x3F201000) //HI3516:0x120A0000 rpi2b:0x3F201000
#define AMBA_UART_DR1 (*(volatile unsigned char *)(RPI_BASE_UART_REGISTER1 + 0x00))
#define AMBA_UART_FR1 (*(volatile unsigned char *)(RPI_BASE_UART_REGISTER1 + 0x18))

void uart_putc_phy(unsigned char c)
{
	//UART_Type *uartRegs = (UART_Type *)UART4_REG_PBASE;
	//while ((uartRegs->USART_ISR & (1<<5)) == 0);
	//uartRegs->USART_TDR = c;
	while (AMBA_UART_FR & (1 << 5));
	AMBA_UART_DR = c;
}

void uart_putc_virt(unsigned char c)
{
	//UART_Type *uartRegs = (UART_Type *)UART_REG_BASE;
	//while ((uartRegs->USART_ISR & (1<<5)) == 0);
	//uartRegs->USART_TDR = c;
	while (AMBA_UART_FR1 & (1 << 5));
	AMBA_UART_DR1 = c;
}
/*---------------------------*/

#if 1
#define REG32(addr) ((volatile UINT32 *)(UINTPTR)(addr))
#define UARTREG(base, reg)  (*REG32((base) + (reg)))
#define UART_FR_TXFF (0x1U << 5)

STATIC VOID UartPutcReg(UINTPTR base, CHAR c)
{
    /* Spin while fifo is full */
    while (UARTREG(base, UART_FR) & UART_FR_TXFF) {}
    UARTREG(base, UART_DR) = c;
}
#else
STATIC VOID UartPutcReg(UINTPTR base, CHAR c)
{
	//UART_Type *uartRegs = (UART_Type *)base;

	//while ((uartRegs->USART_ISR & (1<<7)) == 0); /*等待上个字节发送完毕*/
	//uartRegs->USART_TDR = c;
	while (AMBA_UART_FR1 & (1 << 5));
	AMBA_UART_DR1 = c;
}
#endif

STATIC INLINE UINTPTR uart_to_ptr(UINTPTR n)
{
    (VOID)n;
    return UART_REG_BASE;
}

#if 0
INT32 uart_putc(INT32 port, CHAR c)
{
    UINTPTR base = uart_to_ptr((UINT32)port);
    UartPutcReg(base, c);
    return 1;
}

CHAR uart_fputc(CHAR c, VOID *f)
{
    (VOID)f;
    if (g_uart_fputc_en == 1) {
        if (c == '\n') {
            (VOID)uart_putc(0, '\r');
        }
        return (uart_putc(0, (c)));
    } else {
        return 0;
    }
}
#else
int myputs(const char *s)
{
	while (*s)
	{
		uart_putc_virt(*s);
		s++;
	}
	return 0;
}
void myputhex(unsigned int val)
{
	/* 0x76543210 */
	int i, j;

	myputs("0x");
	for (i = 7; i >= 0; i--)
	{
		j = (val >> (i*4)) & 0xf;
		if ((j >= 0) && (j <= 9))
			uart_putc_virt('0' + j);
		else
			uart_putc_virt('A' + j - 0xA);
	}	
}
#endif

LITE_OS_SEC_BSS STATIC SPIN_LOCK_INIT(g_uartOutputSpin);

STATIC VOID UartPutStr(UINTPTR base, const CHAR *s, UINT32 len)
{
    UINT32 i;
    for (i = 0; i < len; i++) {
        if (*(s + i) == '\n') {
			UartPutcReg(base, *"\r");
        }
		UartPutcReg(base, *(s + i));
    }
}


UINT32 UartPutsReg(UINTPTR base, const CHAR *s, UINT32 len, BOOL isLock)
{
    UINT32 intSave;

    if (g_uart_fputc_en == 0) {
        return 0;
    }

	extern void uart_putc_virt(unsigned char c);
    if (isLock) {
        LOS_SpinLockSave(&g_uartOutputSpin, &intSave);
        UartPutStr(base, s, len);
        LOS_SpinUnlockRestore(&g_uartOutputSpin, intSave);
    } else {
        UartPutStr(base, s, len);
    }

    return len;
}

VOID UartPuts(const CHAR *s, UINT32 len, BOOL isLock)
{
    UINTPTR base = uart_to_ptr(0);
    (VOID)UartPutsReg(base, s, len, isLock);
}

INT32 uart_puts(const CHAR *s, UINTPTR len, VOID *state)
{
    (VOID)state;
    UINTPTR i;

    for (i = 0; i < len; i++) {
        if (*(s + i) != '\0') {
            if (*(s + i) == '\n') {
                (VOID)uart_fputc('\r', NULL);
            }

            (VOID)uart_fputc(*(s + i), NULL);
        }
    }

    return (INT32)len;
}
#if 0
VOID uart_handler(VOID)
{PRINT_RELEASE("%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    CHAR c;
    UINTPTR base = uart_to_ptr(0);

    c = UARTREG(base, UART_DR);

    switch (c) {
        case '\r':
        case '\n':
            if (g_inputIdx < CMD_LENGTH - 1) {
                g_inputCmd[g_inputIdx++] = '\0';
                LOS_EventWrite(&g_stShellEvent, 0x1);
                (VOID)uart_putc(0, '\r');
                (VOID)uart_putc(0, '\n');
            }
            break;
        case 0x8:   /* backspace */
        case 0x7f:  /* delete */
            if (g_inputIdx > 0) {
                g_inputIdx--;
                (VOID)uart_putc(0, '\b');
                (VOID)uart_putc(0, ' ');
                (VOID)uart_putc(0, '\b');
            }
            break;
        default:
            if (g_inputIdx < CMD_LENGTH - 1) {
                (VOID)uart_putc(0, c);
                g_inputCmd[g_inputIdx++] = c;
            }
    }
}

VOID uart_early_init(VOID)
{
    /* enable uart transmit */
    UARTREG(UART_REG_BASE, UART_CR) = (1 << 8) | (1 << 0);
}

VOID uart_init(VOID)
{PRINT_RELEASE(">>>>>%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
    UINT32 ret;

    /* uart interrupt priority should be the highest in interrupt preemption mode */
    ret = LOS_HwiCreate(NUM_HAL_INTERRUPT_UART, 0, 0, (HWI_PROC_FUNC)uart_handler, NULL);
    if (ret != LOS_OK) {
        PRINT_ERR("%s,%d, uart interrupt created error:%x\n", __FUNCTION__, __LINE__, ret);
    } else {
        /* clear all irqs */
        UARTREG(UART_REG_BASE, UART_ICR) = 0x3ff;
		PRINT_RELEASE(">>>>>%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
        /* set fifo trigger level */
        UARTREG(UART_REG_BASE, UART_IFLS) = 0;

        /* enable rx interrupt */
        UARTREG(UART_REG_BASE, UART_IMSC) = (1 << 4 | 1 << 6);

        /* enable receive */
        UARTREG(UART_REG_BASE, UART_CR) |= (1 << 9);

        HalIrqUnmask(NUM_HAL_INTERRUPT_UART);
    }
}
#else  //原来kernel\liteos_a\platform\uart\amba_pl011\amba_pl011.c 的这部分代码用下面的代码替换。

VOID uart_early_init(VOID)
{
    /* enable uart transmit */
}

VOID uart_init(VOID)
{
}

#define FIFO_SIZE    128

static irqreturn_t BCM2836_uart_irq(int irq, void *data) //产生中断时，这个函数调用
{//PRINT_RELEASE(">>>>>%s %s %d %X\n",__FILE__,__FUNCTION__,__LINE__,*(volatile UINT32 *)((UINTPTR)IO_DEVICE_ADDR(0x3F201040)));//0x10
	//WRITE_UINT32((0x3ff), UART_REG_BASE + UART_IMSC);/* interrupt clear register */
	//UARTREG(UART_REG_BASE, UART_IMSC) = 0x3ff;


	char buf[FIFO_SIZE];
	unsigned int count = 0;
	struct BCM2836_port *port = NULL;
	struct uart_driver_data *udd = (struct uart_driver_data *)data;
	UART_Type *uartRegs;
    uint32_t status;
	
	if (udd == NULL) {
		uart_error("udd is null!\n");
		return IRQ_HANDLED;
	}
	port = (struct BCM2836_port *)udd->private;

	uartRegs = (UART_Type *)port->phys_base;

    //status = uartRegs->USART_ISR;//替换，底下这两句来自Z:\bright\harmony\kernel\liteos_a\platform\uart\amba-pl011-lagacy\uart.c
    READ_UINT32(status, UART_REG_BASE + UART_FR);
	/*PRINT_RELEASE(">>>>>%s %s %d %X\n",__FILE__,__FUNCTION__,__LINE__,UARTREG(UART_REG_BASE,UART_DR));
	PRINT_RELEASE(">>>>>%s %s %d %X\n",__FILE__,__FUNCTION__,__LINE__,UARTREG(UART_REG_BASE,UART_FR));
	PRINT_RELEASE(">>>>>%s %s %d %X\n",__FILE__,__FUNCTION__,__LINE__,UARTREG(UART_REG_BASE,UART_IMSC));
	PRINT_RELEASE(">>>>>%s %s %d %X\n",__FILE__,__FUNCTION__,__LINE__,UARTREG(UART_REG_BASE,UART_RIS));
	PRINT_RELEASE(">>>>>%s %s %d %X\n",__FILE__,__FUNCTION__,__LINE__,UARTREG(UART_REG_BASE,UART_MIS));*/
	if ((UARTREG(UART_REG_BASE,UART_FR)&(1<<4)) == 0) {
        do {
			/*PRINT_RELEASE(">>>>>%s %s %d %X\n",__FILE__,__FUNCTION__,__LINE__,UARTREG(UART_REG_BASE,UART_DR));
			PRINT_RELEASE(">>>>>%s %s %d %X\n",__FILE__,__FUNCTION__,__LINE__,UARTREG(UART_REG_BASE,UART_FR));
			PRINT_RELEASE(">>>>>%s %s %d %X\n",__FILE__,__FUNCTION__,__LINE__,UARTREG(UART_REG_BASE,UART_IMSC));
			PRINT_RELEASE(">>>>>%s %s %d %X\n",__FILE__,__FUNCTION__,__LINE__,UARTREG(UART_REG_BASE,UART_RIS));
			PRINT_RELEASE(">>>>>%s %s %d %X\n",__FILE__,__FUNCTION__,__LINE__,UARTREG(UART_REG_BASE,UART_MIS));*/

            buf[count++] = UARTREG(UART_REG_BASE,UART_DR);//*(volatile UINT32 *)((UINTPTR)(UART_REG_BASE + UART_DR)); //去读取硬件得到数据
            //PRINT_RELEASE(">>>>>%s %c %d\n",__FILE__,buf[count],count);
            if (udd->num != CONSOLE_UART) {
				PRINT_RELEASE("%s %s %d, udd->num = %d, CONSOLE_UART = %d\n", __FILE__, __FUNCTION__, __LINE__, udd->num, CONSOLE_UART);
                continue;
            }
            if (CheckMagicKey(buf[count - 1])) { //数据放在buf里
                goto end;
            }

			if (buf[count-1] == '\r') //对windows和liteos回车换行的处理
				buf[count-1] = '\n';
			//status = uartRegs->USART_ISR;
			//READ_UINT32(status, UART_REG_BASE + UART_FR);
		} while (UARTREG(UART_REG_BASE,UART_DR));
		//buf[count] = '\0';
		//PRINT_RELEASE("%s %s %d, udd->recv %d, %s\n", __FILE__, __FUNCTION__, __LINE__, count, buf);//udd->recv 1,
        udd->recv(udd, buf, count); //调用udd里的recv函数把数据发送给上一级
    }
	//WRITE_UINT32((1 << 4) | (1 << 6), UART_REG_BASE + UART_CLR);/* interrupt clear register */
	UARTREG(UART_REG_BASE, UART_ICR) = 0x3ff;
	//PRINT_RELEASE(">>>>>%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
end:
	/* clear all interrupt */
	return 0;
}


static int BCM2836_config_in(struct uart_driver_data *udd)
{
	return 0;
}

static int BCM2836_startup(struct uart_driver_data *udd) 
{PRINT_RELEASE(">>>>>%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
	/*---------3516uart_init---------*/
#if 0
    UINT32 ret;

    /* 1.uart interrupt priority should be the highest in interrupt preemption mode */
    ret = LOS_HwiCreate(NUM_HAL_INTERRUPT_UART, 0, 0, (HWI_PROC_FUNC)uart_handler, NULL);
    if (ret != LOS_OK) {
        PRINT_ERR("%s,%d, uart interrupt created error:%x\n", __FUNCTION__, __LINE__, ret);
    } else {
        /* 2.clear all irqs */
        UARTREG(UART_REG_BASE, UART_ICR) = 0x3ff;
		PRINT_RELEASE(">>>>>%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
        /* 3.set fifo trigger level */
        UARTREG(UART_REG_BASE, UART_IFLS) = 0;

        /* 4.enable rx interrupt */
        UARTREG(UART_REG_BASE, UART_IMSC) = (1 << 4 | 1 << 6);

        /* 5.enable receive */
        UARTREG(UART_REG_BASE, UART_CR) |= (1 << 9);

        HalIrqUnmask(NUM_HAL_INTERRUPT_UART);//6.
    	}
	---------3516uart_init---------*/
#endif

	int ret = 0;
	struct BCM2836_port *port = NULL;

	if (udd == NULL) {
		uart_error("udd is null!\n");
		return -EFAULT;
	}

	port = (struct BCM2836_port *)udd->private;//*private是一个指针，指向 struct {enable,phys_base,irq_num,*udd}
	if (!port) {
		uart_error("port is null!");
		return -EFAULT;
	}
	
	//UART_Type *uartRegs;  //韦老师的寄存器地址	
	//uartRegs = (UART_Type *)port->phys_base; //韦老师的寄存器地址
	
	/* enable the clock */
	LOS_TaskLock();
	//uart_clk_cfg(udd->num, true); //use for hi3518
	LOS_TaskUnlock();
	

    /* enable the clock */
	//*(volatile UINT32 *)((UINTPTR)IO_DEVICE_ADDR(0x3F201024)) = 1;
	//*(volatile UINT32 *)((UINTPTR)IO_DEVICE_ADDR(0x3F201028)) = 40;

	
	/*--------重要--------*/
	ret = request_irq(port->irq_num, (irq_handler_t)BCM2836_uart_irq,0, "uart_dw", udd);  //去注册一个串口的接收中断函数
	//ret = request_irq(19, (irq_handler_t)BCM2836_uart_irq,0, "uart_dw", udd);
	PRINT_RELEASE(">>>>>port->irq_num:%d\n",port->irq_num);
	/* 1.uart interrupt priority should be the highest in interrupt preemption mode */
    //ret = LOS_HwiCreate(NUM_HAL_INTERRUPT_UART, 0, 0, (HWI_PROC_FUNC)uart_handler, NULL);

	/* 2.clear all irqs */
    UARTREG(UART_REG_BASE, UART_ICR) = 0x3ff;
    //*(volatile UINT32 *)((UINTPTR)IO_DEVICE_ADDR(0x3F201044)) = 0x3ff;


	/* disable FIFO mode */
	//uartRegs->USART_CR1 &= ~(1<<29);
	//*(volatile UINT32 *)((UINTPTR)IO_DEVICE_ADDR(0x3F20102C)) = 0x60;
	UARTREG(UART_REG_BASE, UART_LCR_H) = (1 << 6 | 1 << 5| 1 << 4); //不开5、6乱码，不开4崩溃

	/* 3.set fifo trigger level */
	//*(volatile UINT32 *)((UINTPTR)IO_DEVICE_ADDR(0x3F201034)) = 0x0;
	UARTREG(UART_REG_BASE, UART_IFLS) = 0;

	

	/* enable rx interrupt
	 * RXFNEIE: RXFIFO not empty interrupt enable
	 */
	/* 4.enable rx interrupt 开启串口接收中断，第4位*/
    UARTREG(UART_REG_BASE, UART_IMSC) = (1 << 4 | 1 << 6); //*(volatile UINT32 *)((UINTPTR)IO_DEVICE_ADDR(0x3F201038)) = 0x10;

	/* enable uart */	
	/* 5.enable receive */
    UARTREG(UART_REG_BASE, UART_CR) |= (1 << 9); //*(volatile UINT32 *)((UINTPTR)IO_DEVICE_ADDR(0x3F201030)) = 0x301;


	//HalIrqUnmask(NUM_HAL_INTERRUPT_UART);//6.	
	*(volatile UINT32 *)((UINTPTR)IO_DEVICE_ADDR(0x3F00B214)) = 0x02000000;//Unmask接收25号中断

	
	
	PRINT_RELEASE(">>>>>%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
	BCM2836_config_in(udd);

	return ret;
}

static int BCM2836_shutdown(struct uart_driver_data *udd)
{PRINT_RELEASE(">>>>>%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
	return 0;
}

static int BCM2836_start_tx(struct uart_driver_data *udd, const char *buf, size_t count)
{//PRINT_RELEASE(">>>>>%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
	unsigned int tx_len = count;
	struct BCM2836_port *port = NULL;
	char value;
	unsigned int i;
	int ret = 0;

	if (udd == NULL) {
		uart_error("udd is null!\n");
		return -EFAULT;
	}
	port = (struct BCM2836_port *)udd->private;
	if (!port) {
		uart_error("port is null!");
		return -EFAULT;
	}
	/* UART_WITH_LOCK: there is a spinlock in the function to write reg in order. */
	/* UART_WITH_LOCK: 函数中有一个自旋锁来按顺序写入reg. */
	for (i = 0; i < tx_len; i++ ){
		ret = LOS_CopyToKernel((void *)&value, sizeof(char),(void *)(buf++), sizeof(char));
		if (ret) {
			return i;
		}
		(void)UartPutsReg(port->phys_base, &value, 1, UART_WITH_LOCK);
	}
	//PRINT_RELEASE(">>>>>%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
	return count;
}

static int BCM2836_config(struct uart_driver_data *udd)
{PRINT_RELEASE(">>>>>%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
	return BCM2836_config_in(udd);
}

static struct uart_ops BCM2836_uops = {
	.startup        = BCM2836_startup,
	.shutdown       = BCM2836_shutdown,
	.start_tx       = BCM2836_start_tx,
	.config         = BCM2836_config,
};

#define MAX_DEV_NAME_SIZE  32
extern const struct file_operations_vfs uartdev_fops;
extern struct uart_driver_data *get_udd_by_unit(int unit);

static int BCM2836_attach(device_t self)
{PRINT_RELEASE(">>>>>%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
	struct resource *res = NULL;
	char dev_name[MAX_DEV_NAME_SIZE];
	struct BCM2836_port *port = NULL;
	int unit = device_get_unit(self);
	struct uart_softc *sc = device_get_softc(self);
	struct uart_driver_data *udd = sc->udd;

	if (udd == NULL) {
		uart_error("BCM2836_attach get uart driver data err!");
		return -1;
	}
	port = (struct BCM2836_port *)LOS_MemAlloc(m_aucSysMem0, sizeof(struct BCM2836_port));  //获取资源
	if (!port) {
		return -1;
	}
	memset_s(port, sizeof(struct BCM2836_port), 0, sizeof(struct BCM2836_port));
	res = bus_alloc_resource_any(self, SYS_RES_MEMORY, &unit, 0);  //从设备获取内存资源
	if (!res) {
		goto err;
	}
	port->phys_base = (unsigned long)(uintptr_t)ioremap(res->start, res->count); //使用ioremap获得虚拟地址【任务：将physical 改为 virtual】
	if (!port->phys_base) {
		goto err;
	}
	res = bus_alloc_resource_any(self, SYS_RES_IRQ, &unit, 0); //从这个设备里获得中断资源
	if (!res) {
		goto err;
	}
	
	port->irq_num = res->start;
	if (port->irq_num == LOS_NOK) {
		goto err;
	}

	udd->private = port;  //*private是一个指针，指向 struct {        enable,phys_base,irq_num,*udd}
	PRINT_RELEASE(">>>>>%d %x %d\n",port->enable,port->phys_base,port->irq_num); //>>>>>0 6c201000 25
	udd->ops = &BCM2836_uops;//*ops也是一个指针，指向 struct {BCM2836_startup,BCM2836_shutdown,BCM2836_start_tx,BCM2836_config}
	port->udd = udd;
	udd->recv = uart_recv_notify; //udd->recv的函数
	udd->count = 0;
	memset_s(dev_name, MAX_DEV_NAME_SIZE, 0, MAX_DEV_NAME_SIZE);
	snprintf_s(dev_name, MAX_DEV_NAME_SIZE, MAX_DEV_NAME_SIZE - 1, "/dev/uartdev-%d", udd->num); //dev_name = /dev/uartdev-0
	if (register_driver(dev_name, &uartdev_fops, 0666, udd)) { //【核心】然后注册驱动程序，驱动名=》驱动程序结构体
		uart_error("gen /dev/uartdev-%d fail!\n", udd->num);
		goto err;
	}
	return 0;
err:
	iounmap((void *)(uintptr_t)port->phys_base);
	(VOID)LOS_MemFree(m_aucSysMem0, port);
	port =  NULL;
	return -1;
}

static int BCM2836_probe(device_t self)
{
	return (BUS_PROBE_DEFAULT);
}

static int BCM2836_detach(device_t self)
{PRINT_RELEASE(">>>>>%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
	struct uart_softc *sc = device_get_softc(self);
	struct uart_driver_data *udd = sc->udd;
	struct BCM2836_port *port = NULL;
	char dev_name[MAX_DEV_NAME_SIZE];
	if (udd == NULL || (udd->state != UART_STATE_USEABLE)) {
		uart_error("BCM2836_detach uart driver data state invalid!");
		return -1;
	}

	(void)memset_s(dev_name, MAX_DEV_NAME_SIZE, 0, MAX_DEV_NAME_SIZE);
	(void)snprintf_s(dev_name, MAX_DEV_NAME_SIZE, MAX_DEV_NAME_SIZE - 1, "/dev/uartdev-%d", udd->num);
	if (unregister_driver(dev_name)) {
		uart_error("BCM2836_detach unregister /dev/uartdev-%d fail!\n", udd->num);
	}
	port = udd->private;
	if (port == NULL) {
		return -1;
	}
	if (port->phys_base) {
		iounmap((void *)(uintptr_t)port->phys_base);
		port->phys_base = 0;
	}
	(VOID)LOS_MemFree(m_aucSysMem0, port);
	udd->private = NULL;
	return 0;
}

static device_method_t uart_methods[] = 
{
	/* Device interface */
	DEVMETHOD(device_probe, BCM2836_probe),
	DEVMETHOD(device_attach, BCM2836_attach),
	DEVMETHOD(device_detach, BCM2836_detach),
	DEVMETHOD(device_shutdown, bus_generic_shutdown),
	DEVMETHOD_END
};

static driver_t uart_driver = 
{
	.name = "uart",
	.methods = uart_methods,
	.size = sizeof(struct uart_softc),
};

static devclass_t uart_devclass;
DRIVER_MODULE(uart, nexus, uart_driver, uart_devclass, 0, 0);

int uart_dev_init(void)
{
	return driver_module_handler(NULL, MOD_LOAD, &uart_nexus_driver_mod);
}

void uart_dev_exit(void)
{
	driver_module_handler(NULL, MOD_UNLOAD, &uart_nexus_driver_mod);
}
#endif
