

# 7 Interrupts

```
//#define PBASE 0x20000000 /* "BCM2835.h"  Original B,A,A+,B+ */
#define PBASE 0x3F000000 /* "BCM2836.h"  Raspberriy Pi 2 */

#define IRQ_BASIC           (PBASE+0x0000B200)	/* IRQ basic pending */
#define IRQ_PEND1           (PBASE+0x0000B204)	/* IRQ pending 1 */
#define IRQ_PEND2           (PBASE+0x0000B208)	/* IRQ pending 2 */
#define IRQ_FIQ_CONTROL     (PBASE+0x0000B20C)	/* FIQ control */
#define IRQ_ENABLE1         (PBASE+0x0000B210)	/* Enable IRQs 1 */
#define IRQ_ENABLE2         (PBASE+0x0000B214)	/* Enable IRQs 2 */
#define IRQ_ENABLE_BASIC    (PBASE+0x0000B218)	/* Enable Basic IRQs */
#define IRQ_DISABLE1        (PBASE+0x0000B21C)	/* Disable IRQs 1 */
#define IRQ_DISABLE2        (PBASE+0x0000B220)	/* Disable IRQs 2 */
#define IRQ_DISABLE_BASIC   (PBASE+0x0000B224)	/* Disable Basic IRQs */
```

## ARM外设中断表丨ARM peripherals interrupts table.

表里列出了所有的中断，这些中断可以来自于外设，这些外设可以被ARM处理

| #    | IRQ 0-15             | #    | IRQ 16-31 | #    | IRQ 32-47       | #    | IRQ 48-63   |
| ---- | -------------------- | ---- | --------- | ---- | --------------- | ---- | ----------- |
| 0    |                      | 16   |           | 32   |                 | 48   | smi         |
| 1    | system timer match 1 | 17   |           | 33   |                 | 49   | gpio_int[0] |
| 2    |                      | 18   |           | 34   |                 | 50   | gpio_int[1] |
| 3    | system timer match 3 | 19   |           | 35   |                 | 51   | gpio_int[2] |
| 4    |                      | 20   |           | 36   |                 | 52   | gpio_int[3] |
| 5    |                      | 21   |           | 37   |                 | 53   | i2c_int     |
| 6    |                      | 22   |           | 38   |                 | 54   | spi_int     |
| 7    |                      | 23   |           | 39   |                 | 55   | pcm_int     |
| 8    |                      | 24   |           | 40   |                 | 56   |             |
| 9    | USB controller       | 25   |           | 41   |                 | 57   | uart_int    |
| 10   |                      | 26   |           | 42   |                 | 58   |             |
| 11   |                      | 27   |           | 43   | i2c_spi_slv_int | 59   |             |
| 12   |                      | 28   |           | 44   |                 | 60   |             |
| 13   |                      | 29   | Aux int   | 45   | pwa0            | 61   |             |
| 14   |                      | 30   |           | 46   | pwa1            | 62   |             |
| 15   |                      | 31   |           | 47   |                 | 63   |             |

| 0     | ARM Timer                                                    |
| ----- | ------------------------------------------------------------ |
| **1** | **ARM Mailbox**                                              |
| **2** | **ARM Doorbell 0**                                           |
| **3** | **ARM Doorbell 1**                                           |
| **4** | **GPU0 halted (Or GPU1 halted if bit 10 of control register 1 is set)** |
| **5** | **GPU1 halted**                                              |
| **6** | **Illegal access type 1**                                    |
| **7** | **Illegal access type 0**                                    |

## 查询

## 0x200 IRQ basic pending丨Basic pending register

这个寄存器是只读的，查询中断状态。

0x200显示哪个中断是挂起的。为了加快中断处理速度，一些“正常”中断状态位被添加到这个寄存器中。

这使得'IRQ pending base'寄存器与其他'base'中断寄存器不同。

| Name: IRQ pend base | Address: 0x200 | Reset: 0x000                                                 |
| ------------------- | -------------- | ------------------------------------------------------------ |
| Bit(s)              | R/W            | Function                                                     |
| 31:21               | -              | <unused>                                                     |
| 20                  | R              | GPU IRQ 62                                                   |
| 19                  | R              | GPU IRQ 57                                                   |
| 18                  | R              | GPU IRQ 56                                                   |
| 17                  | R              | GPU IRQ 55                                                   |
| 16                  | R              | GPU IRQ 54                                                   |
| 15                  | R              | GPU IRQ 53                                                   |
| 14                  | R              | GPU IRQ 19                                                   |
| 13                  | R              | GPU IRQ 18                                                   |
| 12                  | R              | GPU IRQ 10                                                   |
| 11                  | R              | GPU IRQ 9                                                    |
| 10                  | R              | GPU IRQ 7                                                    |
| 9                   | R              | One or more bits set in pending register 2                   |
| 8                   | R              | One or more bits set in pending register 1                   |
| 7                   | R              | Illegal access type 0 IRQ pending                            |
| 6                   | R              | Illegal access type 1 IRQ pending                            |
| 5                   | R              | GPU1 halted IRQ pending                                      |
| 4                   | R              | GPU0 halted IRQ pending<br/>(Or GPU1 halted if bit 10 of control register 1 is set) |
| 3                   | R              | ARM Doorbell 1 IRQ pending                                   |
| 2                   | R              | ARM Doorbell 0 IRQ pending                                   |
| 1                   | R              | ARM Mailbox IRQ pending                                      |
| 0                   | R 0x1          | ARM Timer IRQ pending                                        |

### GPU IRQ x (10,11~20)

这些位是GPU的直接中断。它们被选择为最可能对ARM有用的中断。GPU中断选择的是7、9、10、18、19、53、54、55、56、57、62。详细信息请参见 GPU interrupts table

### Bits set in pending registers (8,9)

These bits indicates if there are bits set in the pending 1/2 registers. The pending 1/2 registers hold ALL interrupts 0..63 from the GPU side. Some of these 64 interrupts are also connected to the basic pending register. Any bit set in pending register 1/2 which is NOT connected to the basic pending register causes bit 8 or 9 to set. Status bits 8 and 9 should be seen as "There are some interrupts pending which you don't know about. They are in pending register 1 /2."

这些位表明，如果在0x204、0x208中是否设置了位。0x204、0x208保持所有来自GPU的中断0 . .63不变。这64个中断中的一些还连接到0x200。在0x204、0x208中设置的任何位没有连接到0x200，都会导致第8位或第9位被设置。

状态位8和9应该被视为“有一些中断正在挂起，但你不知道。”它们在0x204、0x208中。”

### Illegal access type-0 IRQ (7)

非法访问类型-0 IRQ (7)

This bit indicate that the address/access error line from the ARM processor has generated an interrupt. That signal is asserted when either an address bit 31 or 30 was high or when an access was seen on the ARM Peripheral bus. The status of that signal can be read from Error/HALT status register bit 2.

这个位表示来自ARM处理器的地址/访问错误行已经产生了一个中断。当地址位31或30高或在ARM外围总线上看到访问时，该信号被断言。该信号的状态可以从错误/暂停状态寄存器第2位读取。

### Illegal access type-1 IRQ (6)

非法访问类型-1IRQ (6)

This bit indicates that an address/access error is seen in the ARM control has generated an interrupt.
That can either be an address bit 29..26 was high or when a burst access was seen on the GPU
Peripheral bus. The status of that signal can be read from Error/HALT status register bits 0 and 1.

这个位指示一个地址/访问错误被看到在ARM控制已经产生了一个中断。

这可以是一个地址位29..26是高的，或者是GPU上出现了突发访问

外围总线。该信号的状态可以从错误/暂停状态寄存器0和1位读取。

### GPU-1 halted IRQ (5)

This bit indicate that the GPU-1 halted status bit has generated an interrupt. The status of that signal can be read from Error/HALT status register bits 4.

这个位表示GPU-1暂停状态位已经产生了一个中断。该信号的状态可以从错误/停止状态寄存器位4读取。

### GPU-0 (or any GPU) halted IRQ (4)

This bit indicate that the GPU-0 halted status bit has generated an interrupt. The status of that signal can be read from Error/HALT status register bits 3.

这个位表示图形处理器0停止状态位已经产生了一个中断。该信号的状态可以从错误/停止状态寄存器位3读取。

In order to allow a fast interrupt (FIQ) routine to cope with GPU 0 OR GPU-1 there is a bit in control register 1 which, if set will also route a GPU-1 halted status on this bit

为了允许一个快速中断(FIQ)程序来处理GPU 0或GPU-1，在控制寄存器1中有一个位，如果设置，也会在这个位上路由一个GPU-1暂停状态

### Standard peripheral IRQs (0,1,2,3)

These bits indicate if an interrupt is pending for one of the ARM control peripherals.

这些位指示一个ARM控制外设的中断是否挂起。

------

## 0x204 IRQ pending 1丨GPU pending 1 register.

GPU 挂起中的寄存器 1。

| Name:IRQ pend base | Address: 0x20C | Reset: 0x000                             |
| ------------------ | -------------- | ---------------------------------------- |
| Bit(s)             | R/W            | Function                                 |
| 31:0               | R              | IRQ 0-31位，挂起中的来源。详见 IRQ table |

This register holds ALL interrupts 0..31 from the GPU side. Some of these interrupts are also connected to the basic pending register. Any interrupt status bit in here which is NOT connected to the basic pending will also cause bit 8 of the basic pending register to be set. That is all bits except 7, 9, 10, 18, 19.

这个寄存器保存所有中断0..31来自GPU。其中一些中断还连接到基本挂起寄存器（0x200）。这里没有连接到基本挂起寄存器的任何中断状态位也会导致设置基本挂起寄存器的第8位。这是除了7、9、10、18、19之外的所有位。

## 0x208 IRQ pending 2丨GPU pending 2 register.

GPU 挂起中的寄存器 2。

| Name:IRQ pend base | Address: 0x20C | Reset: 0x000                              |
| ------------------ | -------------- | ----------------------------------------- |
| Bit(s)             | R/W            | Function                                  |
| 31:0               | R              | IRQ 32-63位，挂起中的来源。详见 IRQ table |

This register holds ALL interrupts 32..63 from the GPU side. Some of these interrupts are also connected to the basic pending register. Any interrupt status bit in here which is NOT connected to the basic pending will also cause bit 9 of the basic pending register to be set. That is all bits except .register bits 21..25, 30 (Interrupts 53..57,62).

这个寄存器保存所有中断32..63来自GPU。其中一些中断还连接到基本挂起寄存器（0x200）。这里任何没有连接到基本挂起寄存器的中断状态位也会导致设置基本挂起寄存器的第9位，这是所有的位了，除了21..25, 30位。(Interrupts 53..57、62)。

## 开启

## 0x20C FIQ control     丨FIQ register.

The FIQ register control which interrupt source can generate a FIQ to the ARM. Only a single interrupt can be selected.

0x20C 控制由哪一个中断源可以生成FIQ到ARM。只能选择一个中断。

| Name: IRQ | Address: 0x20C | Reset: 0x000                     |
| --------- | -------------- | -------------------------------- |
| Bit(s)    | R/W            | Function                         |
| 31:8      | R              | <unused>                         |
| 7         | R              | FIQ enable将此位设置为1以使能FIQ |
| 6:0       | R/W            | Select FIQ Source丨选择FIQ源     |

### FIQ Source

The FIQ source values 0-63 correspond to the GPU interrupt table. (See above)

FIQ源值0 ~ 63对应GPU中断表。(见上文)

The following values can be used to route ARM specific interrupts to the FIQ vector/routine:

下面的值 被用来 路由ARM具体的中断 到FIQ vector/routine:中断向量表？

| FIQ index | Source                             |
| --------- | ---------------------------------- |
| 0-63      | Source                             |
| 64        | GPU Interrupts (See GPU IRQ table) |
| 65        | ARM Timer interrupt                |
| 66        | ARM Mailbox interrupt              |
| 67        | ARM Doorbell 0 interrupt           |
| 68        | ARM Doorbell 1 interrupt           |
| 69        | GPU0 Halted interrupt (Or GPU1)    |
| 70        | GPU1 Halted interrupt              |
| 71        | Illegal access type-1 interrupt    |
| 72-127    | Illegal access type-0 interrupt    |

## 0x210 Enable IRQs 1丨Interrupt enable register 1.

## 0x214 Enable IRQs 2丨Interrupt enable register 2.

## 0x218 Enable Basic IRQs丨Base Interrupt enable register.

| Name:中断使能寄存器1       | Address: 0x210     | Reset: 0x000                              |
| -------------------------- | ------------------ | ----------------------------------------- |
| Bit(s)                     | R/W                | Function                                  |
| 31:0                       | R/Wbs              | 设置31:0启用IRQ中断源（见上方IRQ table）  |
| **Name:中断使能寄存器2**   | **Address: 0x214** | **Reset: 0x000**                          |
| 31:0                       | R/Wbs              | 设置63:32启用IRQ中断源（见上方IRQ table） |
| **Name: 中断使能寄存器 3** | **Address: 0x218** | **Reset: 0x000**                          |
| 31:8                       | R/Wbs              | <Unused>                                  |
| 7                          | R/Wbs              | 启用访问错误类型-0 IRQ                    |
| 6                          | R/Wbs              | 启用访问错误类型-1 IRQ                    |
| 5                          | R/Wbs              | 启用GPU 1 Halted IRQ                      |
| 4                          | R/Wbs              | 启用GPU 0 Halted IRQ                      |
| 3                          | R/Wbs              | 使能ARM Doorbell 1 IRQ                    |
| 2                          | R/Wbs              | 使能ARM Doorbell 0 IRQ                    |
| 1                          | R/Wbs              | 使能ARM Mailbox IRQ                       |
| 0                          | R/Wbs              | 使能ARM Timer IRQ                         |

Writing a 1 to a bit will set the corresponding IRQ enable bit. All other IRQ enable bits are unaffected. Only bits which are enabled can be seen in the interrupt pending registers. There is no provision here to see if there are interrupts which are pending but not enabled.

将1写入对应的Bit(s)，将启用相应的IRQ。所有其他IRQ启用位不受影响。只有启用的位可以在中断挂起中的寄存器（interrupt pending registers）中看到。这里没有检查是否有挂起但未启用的中断。

## 关闭

## 0x21C Disable IRQs 1

## 0x220 Disable IRQs 2

## 0x224 Disable Basic IRQs

# 12 System Timer 0x3F003000

The System Timer peripheral provides four 32-bit timer channels and a single 64-bit free running counter. Each channel has an output compare register, which is compared against the 32 least significant bits of the free running counter values. When the two values match, the system timer peripheral generates a signal to indicate a match for the appropriate channel. The match signal is then fed into the interrupt controller. The interrupt service routine then reads the output compare register and adds the appropriate offset for the next timer tick. The free running counter is driven by the timer clock and stopped whenever the processor is stopped in debug mode.

系统计时器外围设备提供四个32位计时器通道和一个单独的64位空闲运行计数器。每个通道都有一个输出比较寄存器，它与自由运行计数器值的32个最低有效位进行比较。当两个值匹配时，系统定时器外围设备产生一个信号来指示适当通道的匹配。然后，匹配信号被送入中断控制器。然后，中断服务程序读取输出比较寄存器，并为下一次计时器计时添加适当的偏移量。空闲运行计数器由计时器时钟驱动，并且在调试模式下处理器停止时停止。

The Physical (hardware) base address for the system timers is 0x7E003000.系统计时器的物理(硬件)基址是0x7E003000。

## 0x0 CS System Timer Control/Status

This register is used to record and clear timer channel comparator matches. The system timer match bits are routed to the interrupt controller where they can generate an interrupt.

这个寄存器用来记录和清除计时器通道比较器匹配。系统计时器匹配位被路由到中断控制器，在那里它们可以产生一个中断。

The M0-3 fields contain the free-running counter match status. Write a one to the relevant bit to clear the match detect status bit and the corresponding interrupt request line.

M0-3字段包含自由运行的计数器匹配状态。写一个1到相关的位，以清除匹配检测状态位和相应的中断请求行。

| Bit(s) | Field Name | Description                                                  | Type | Reset |
| ------ | ---------- | ------------------------------------------------------------ | ---- | ----- |
| 31:4   |            | Reserved - Write as 0, read as don't care                    |      |       |
| 3      | M3         | System Timer Match 3 <br/>0 = No Timer 3 match since last cleared.<br/>1 = Timer 3 match detected. | RW   | 0x0   |
| 2      | M2         | System Timer Match 2<br/>0 = No Timer 2 match since last cleared.<br/>1 = Timer 2 match detected. | RW   | 0x0   |
| 1      | M1         | System Timer Match 1<br/>0 = No Timer 1 match since last cleared.<br/>1 = Timer 1 match detected. | RW   | 0x0   |
| 0      | M0         | System Timer Match 0 系统定时器匹配0<br/>0 = No Timer 0 match since last cleared.<br/>1 = Timer 0 检测到匹配. | RW   | 0x0   |

## 0x4 CLO System Timer Counter Lower 32 bits

The system timer free-running counter lower register is a read-only register that returns the current value of the lower 32-bits of the free running counter.

系统计时器自由运行计数器下寄存器是一个只读寄存器，它返回自由运行计数器下32位的当前值。

31:0 CNT Lower 32-bits of the free running counter value.降低自由运行计数器值的32位。

## 0x8 CHI System Timer Counter Higher 32 bits

The system timer free-running counter higher register is a read-only register that returns the current value of the higher 32-bits of the free running counter.

系统计时器自由运行计数器更高寄存器是一个只读寄存器，它返回自由运行计数器更高32位的当前值。

31:0 CNT Higher 32-bits of the free running counter value.更高的自由运行计数器值的32位

## 0xC C0 System Timer Compare 0

## 0x10 C1 System Timer Compare 1

## 0x14 C2 System Timer Compare 2

## 0x18 C3 System Timer Compare 3

The system timer compare registers hold the compare value for each of the four timer channels.系统定时器比较寄存器保存四个定时器通道的比较值。
Whenever the lower 32-bits of the free-running counter matches one of the compare values the corresponding bit in the system timer control/status register is set.
Each timer peripheral (minirun and run) has a set of four compare registers.

当自由运行计数器的较低32位与比较值之一匹配时在系统定时器控制/状态寄存器中设置相应的位。

每个定时器外围设备(minirun和run)都有一组四个比较寄存器。

# 13 UART 0x3F201000

The BCM2835 device has two UARTS. On mini UART and and PL011 UART. This section describes the PL011 UART. For details of the mini UART see 2.2 Mini UART.

BCM2835设备有两个uart。在mini UART和和PL011 UART。介绍PL011 UART。mini UART的详细信息请参见2.2 mini UART。

The PL011 UART is a Universal Asynchronous Receiver/Transmitter. This is the ARM UART (PL011) implementation. The UART performs serial-to-parallel conversion on data characters received from an external peripheral device or modem, and parallel-to-serial conversion on data characters received from the Advanced Peripheral Bus (APB).

PL011 UART是一种通用的异步接收/发送器。这是ARM UART (PL011)的实现。UART对从外部外围设备或调制解调器接收的数据字符执行串行到并行的转换，并对从高级外围总线(APB)接收的数据字符执行并行到串行的转换。

The ARM PL011 UART has some optional functionality which can be included or left out.

ARM PL011 UART有一些可选的功能，可以包含也可以省略。

> The following functionality is not supported :不支持以下功能:
> ● Infrared Data Association (IrDA) 红外数据协会(IrDA)
> ● Serial InfraRed (SIR) protocol Encoder/Decoder (ENDEC) 串行红外(SIR)协议编码器/解码器(ENDEC)
> ● Direct Memory Access (DMA). 直接内存访问(DMA)

The UART provides:UART提供:
• Separate 16x8 transmit and 16x12 receive FIFO memory.分开16x8发送和16x12接收FIFO存储器。
• Programmable baud rate generator.可编程波特率发生器。
• Standard asynchronous communication bits (start, stop and parity). These are added prior to transmission and removed on reception.标准异步通信位(启动、停止和奇偶校验)。这些是在传输前添加的，在接收时删除的。
• False start bit detection.起始位检测错误。
• Line break generation and detection.断行的产生和检测。
• Support of the modem control functions CTS and RTS. However DCD, DSR, DTR,and RI are not supported.支持现代控制功能CTS和RTS。但是DCD, DSR, DTR和RI不支持。
• Programmable hardware flow control.可编程硬件流量控制。
• Fully-programmable serial interface characteristics: 完全可编程串行接口特性:

​		data can be 5, 6, 7, or 8 bits 数据可以是5,6,7，或8位
​		even, odd, stick, or no-parity bit generation and detection 偶、奇、棒或无校验位的产生和检测
​		1 or 2 stop bit generation 1或2位停止位产生
​		baud rate generation, dc up to UARTCLK/16 波特率产生，直流高达UARTCLK/16
The UART clock source and associated dividers are controlled by the Clock Manager.UART时钟源和相关的分配器由时钟管理器控制。
For the in-depth UART overview, please, refer to the ARM PrimeCell UART (PL011)关于UART的深入概述，请参阅ARM PrimeCell UART (PL011)
Revision: r1p5 Technical Reference Manual.修订:r1p5技术参考手册。

## 13.3 UART Interrupts

The UART has one intra-chip interrupt UARTINTR generated as the OR-ed function of the five individual interrupts.

UART有一个芯片内中断UARTINTR 作为五个单独中断的OR-ed函数生成。

• UARTINTR, this is an OR function of the five individual masked outputs:这是五个单独的掩码输出的OR函数:

​		• UART**RX**INTR

​		• UART**TX**INTR
​		• UART**RT**INTR
​		• UART**MS**INTR, that can be caused by:这可能是由
​				— UART**CTS**INTR, because of a change in the nUART**CTS** modem status.因为nUART**CTS** 的状态发生了变化
​				— UART**DSR**INTR, because of a change in the nUART**DSR** modem status.因为nUART**DSR** 的状态发生了变化
​		• UART**EI**NTR, that can be caused by an error in the reception:这可能是由于接收过程中的错误引起的:

​				— UART**OE**INTR, because of an overrun error 因为超程错误
​				— UART**BE**INTR, because of a break in the reception 因为间连续性中断了
​				— UART**PE**INTR, because of a parity error in the received character 由于接收字符的奇偶校验错误
​				— UART**FE**INTR, because of a framing error in the received character.因为接收字符的帧错误。

One can enable or disable the individual interrupts by changing the mask bits in the Interrupt Mask Set/Clear Register, UART_IMSC. Setting the appropriate mask bit HIGH enables the interrupt.

可以通过改变中断掩码设置/清除寄存器UART_IMSC中的掩码位来启用或禁用单个中断。设置适当的掩码位高可以启用中断。

**UART RX INTR:**

The **transmit interrupt** changes state when one of the following events occurs:当下列事件之一发生时，发送中断改变状态:

​		• If the FIFOs are enabled and the transmit FIFO is equal to or lower than the programmed trigger level then the transmit interrupt is asserted HIGH. The transmit interrupt is cleared by writing data to the transmit FIFO until it becomes greater than the trigger level, or by clearing the interrupt.

如果FIFO被启用并且发送FIFO等于或低于程序触发水平，那么发送中断被断言为高。发送中断通过写数据到发送FIFO被清除，直到它变得大于触发级别，或者通过清除中断。

​		• If the FIFOs are disabled (have a depth of one location) and there is no data present in the transmitters single location, the transmit interrupt is asserted HIGH. It is cleared by performing a single write to the transmit FIFO, or by clearing the interrupt.

如果fifo被禁用(有一个位置的深度)并且在发射机的单个位置没有数据存在，发射中断被断言为高。它通过对发送FIFO执行一次写操作或清除中断来清除。

**UART RT INTR:**

The **receive interrupt** changes state when one of the following events occurs:当下列事件之一发生时，接收中断改变状态:

​		• If the FIFOs are enabled and the receive FIFO reaches the programmed trigger level. When this happens, the receive interrupt is asserted HIGH. The receive interrupt is cleared by reading data from the **receive FIFO** until it becomes less than the trigger level, or by clearing the interrupt.

如果FIFO被启用并且接收FIFO达到编程的触发电平。当这种情况发生时，接收中断被断言为高中断。接收中断通过从接收的FIFO读取数据来清除，直到它变得小于触发级别，或者通过清除中断。

​		• If the FIFOs are disabled (have a depth of one location) and data is received thereby filling the location, the receive interrupt is asserted HIGH. The receive interrupt is cleared by performing a single read of the receive FIFO, or by clearing the interrupt.

如果fifo被禁用(有一个位置的深度)并且数据被接收从而填充位置，接收中断被断言为高。接收中断被清除通过执行一个单独的读取接收FIFO，或清除中断。

## 13.4 Register View

The PL011 USRT is mapped on base adderss 0x7E20100. It has the following memory- mapped registers.

```
//#define UART0_BASE 0x20000000 /* "BCM2835.h"  Original B,A,A+,B+ */
#define UART0_BASE 0x3F201000 /* "BCM2836.h"  Raspberriy Pi 2 */

#define UART0_DR     (UART0_BASE+0x00)	//Data Register
#define UART0_RSRECR (UART0_BASE+0x04)
#define UART0_FR     (UART0_BASE+0x18)	//Flag register
#define UART0_ILPR   (UART0_BASE+0x20)	//not in use
#define UART0_IBRD   (UART0_BASE+0x24)	//Integer Baud rate divisor
#define UART0_FBRD   (UART0_BASE+0x28)	//Fractional Baud rate divisor
#define UART0_LCRH   (UART0_BASE+0x2C)	//Line Control register
#define UART0_CR     (UART0_BASE+0x30)	//Control register
#define UART0_IFLS   (UART0_BASE+0x34)	//Interupt FIFO Level Select Register
#define UART0_IMSC   (UART0_BASE+0x38)	//Interupt Mask Set Clear Register
#define UART0_RIS    (UART0_BASE+0x3C)	//Raw Interupt Status Register
#define UART0_MIS    (UART0_BASE+0x40)	//Masked Interupt Status Register
#define UART0_ICR    (UART0_BASE+0x44)	//Interupt Clear Register
#define UART0_DMACR  (UART0_BASE+0x48)	//DMA Control Register
#define UART0_ITCR   (UART0_BASE+0x80)	//Test Control register
#define UART0_ITIP   (UART0_BASE+0x84)	//Integration test input reg
#define UART0_ITOP   (UART0_BASE+0x88)	//Integration test output reg
#define UART0_TDR    (UART0_BASE+0x8C)	//Test Data reg
```

### 0x00 DR丨数据

For words to be transmitted:
if the FIFOs are enabled, data written to this location is pushed onto the transmit FIFO.
if the FIFOs are not enabled, data is stored in the transmitter holding register(the bottom word of the transmit FIFO).
The write operation initiates transmission from the UART. The data is prefixed with a start bit, appended with the appropriate parity bit (if parity is enabled), and a stop bit. The resultant word is then transmitted.
对于要传输的字:如果启用了FIFO，写入到此位置的数据将被推入传输FIFO。如果FIFO未启用，数据将存储在发送器保存寄存器(发送FIFO的底部字)中。
写操作从UART开始传输。数据以开始位为前缀，附加适当的奇偶校验位(如果启用了奇偶校验)和停止位。由此产生的文字就被传送出去。
For received words:
if the FIFOs are enabled, the data byte and the 4-bit status (break, frame,parity, and overrun) is pushed onto the 12-bit wide receive FIFO if the FIFOs are not enabled, the data byte and status are stored in the receiving holding register (the bottom word of the receive FIFO).
收到的话:如果启用了FIFO,数据字节,4比特状态(休息,框架、平价和溢出)推到12位宽接收FIFO如果FIFO不启用,字节的数据和状态存储在接收存储寄存器(接收FIFO的底部)。

Break error. This bit is set to 1 if a break condition was detected, indicating that the received data input was held LOW for longer than a full-word transmission time (defined as start, data,parity and stop bits).
In FIFO mode, this error is associated with the character at the top of the FIFO. When a break occurs,
only one 0 character is loaded into the FIFO. The next character is only enabled after the receive data
input goes to a 1 (marking state), and the next valid start bit is received.

| Bit(s) | Field Name | Description                                                  | Type | Reset |
| ------ | ---------- | ------------------------------------------------------------ | ---- | ----- |
| 31:12  |            | 保留-写为0，读忽略                                           |      |       |
| 11     | OE         | Overrun error. This bit is set to 1 if data is received and the receive FIFO is already full.<br/>This is cleared to 0 once there is an empty space in the FIFO and a new character can be written to it.<br/>溢出错误。如果接收到数据并且接收的FIFO已经满了，则此位设置为1。<br/>这是允许0 FIFO一旦有一个空的空间,可以编写一个新角色。 | RW   | 0x0   |
| 10     | BE         | Break error. This bit is set to 1 if a break condition was detected, indicating that the received data input was held LOW for longer than a full-word transmission time (defined as start, data,parity and stop bits).<br/>In FIFO mode, this error is associated with the character at the top of the FIFO. When a break occurs,<br/>only one 0 character is loaded into the FIFO. The next character is only enabled after the receive data<br/>input goes to a 1 (marking state), and the next valid start bit is received.<br/>打破错误。如果检测到中断条件，则此位设置为1，指示接收到的数据输入保持较低的时间长于全字传输时间(定义为开始、数据、奇偶校验和停止位)。<br/>在FIFO模式下，此错误与FIFO顶部的字符相关联。当中断发生时，<br/>只有一个0字符被加载到FIFO中。下一个字符只有在接收数据<br/>输入变为1(标记状态)，并且接收到下一个有效的起始位后才会启用。 | RW   | 0x0   |
| 9      | RE         | Parity error. When set to 1, it indicates that the parity of the received data character does not match the<br/>parity that the EPS and SPS bits in the Line<br/>Control Register, UART_LCRH select. In FIFO mode, this error is associated with the character at the top of the FIFO.奇偶校验错误。当设置为1时，表示接收到的数据字符的奇偶校验与UART_LCRH选择<br/>控制寄存器的EPS和SPS位的<br/>奇偶校验不匹配。在FIFO模式下，此错误与FIFO顶部的字符有关。 | RO   | 0x0   |
| 8      | FE         | Framing error. When set to 1, it indicates that the received character did not have a valid stop bit (a valid stop bit is 1). In FIFO mode, this error is associated with the character at the top of the FIFO.<br/>框架的错误。当设置为1时，它表示接收到的字符没有一个有效的停止位(一个有效的停止位是1)。在FIFO模式下，这个错误与FIFO顶部的字符相关联。 | RO   | 0x0   |
| 7:0    | DATA       | Receive (read) data character.Transmit (write) data character.<br/>接收(读取)数据字符。传输(写)数据字符。 | RW   | 0x0   |

### 0x4 RSRECR 空

### 0x18 FR丨标记位

| Bit(s) | Field Name | Description                                                  | Type | Reset |
| ------ | ---------- | ------------------------------------------------------------ | ---- | ----- |
| 31:9   |            | 保留-写为0，读忽略                                           |      |       |
| 8      | RI         | 不支持，写为零，读忽略                                       | RW   | 0x0   |
| 7      | TXFE       | Transmit FIFO empty. The meaning of this bit depends on the state of the FEN bit in the Line Control Register, UARTLCR_LCRH.<br/>If the FIFO is disabled, this bit is set when the transmit holding register is empty.<br/>If the FIFO is enabled, the TXFE bit is set when the transmit FIFO is empty. This bit does not indicate if there is data in the transmit shift register<br/>FIFO启用，**1:Transmit FIFO空**；FIFO禁用，0:transmit holding register空。 | RW   | 0x0   |
| 6      | RXFF       | Receive FIFO full. The meaning of this bit depends on the state of the FEN bit in the UARTLCR_ LCRH Register.<br/>If the FIFO is disabled, this bit is set when the receive holding register is full.<br/>If the FIFO is enabled, the RXFF bit is set when the receive FIFO is full.<br/>FIFO启用，**1:Receive FIFO满**；FIFO禁用，0:receive holding register满。 | RO   | 0x0   |
| 5      | TXFF       | Transmit FIFO full. The meaning of this bit depends on the state of the FEN bit in the UARTLCR_ LCRH Register.<br/>If the FIFO is disabled, this bit is set when the transmit holding register is full.<br/>If the FIFO is enabled, the TXFF bit is set when the transmit FIFO is full.<br/>FIFO启用，**1:Transmit FIFO满**；FIFO禁用，0:transmit holding register满。 | RO   | 0x0   |
| 4      | RXFE       | Receive FIFO empty. The meaning of this bit depends on the state of the FEN bit in the UARTLCR_H Register.<br/>If the FIFO is disabled, this bit is set when the receive holding register is empty.<br/>If the FIFO is enabled, the RXFE bit is set when the receive FIFO is empty.<br/>FIFO启用，**1:Receive FIFO空**；FIFO禁用，0:receive holding register空。 | RW   | 0x0   |
| 3      | BUSY       | UART busy. If this bit is set to 1, the UART is busy transmitting data. This bit remains set until the complete byte, including all the stop bits, has been sent from the **shift register**.<br/>This bit is set as soon as the transmit FIFO becomes non-empty, regardless of whether the UART is enabled or not.<br/>1:表示UART正在忙着传输数据。该位保持设置，直到包括所有停止位在内的完整字节从移位寄存器发出为止。<br/>只要transmit FIFO有值，该位=1，而不管UART是否启用 | RW   | 0x0   |
| 2      | DCD        | 不支持，写为零，读忽略                                       | RW   | 0x0   |
| 1      | DSR        | 不支持，写为零，读忽略。                                     | RW   | 0x0   |

### 0x20空 ILPR not in use

### 0x24空 IBRD Integer Baud rate divisor

### 0x28空 FBRD Fractional Baud rate divisor

### 0x2C LCRH丨线路控制

NOTE: The UART_LCRH, UART_IBRD, and UART_FBRD registers must not be changed:when the UART is enabled
when completing a transmission or a reception when it has been programmed to become disabled.

注意:UART_LCRH, UART_IBRD和UART_FBRD寄存器不能改变:当UART被启用时

当完成一个传输或接收时，当它已被编程成为关闭状态。

| Bit(s) | Field Name | Description                                                  | Type | Reset |
| ------ | ---------- | ------------------------------------------------------------ | ---- | ----- |
| 31:8   |            | 保留-写为0，读忽略                                           |      |       |
| 7      | SPS        | Stick parity select.<br/>0 = stick parity is disabled 禁用奇偶校验<br/>1 = either: 如果EPS位为0，则奇偶校验位为传输和检查为1<br/>if the EPS bit is 0 then the parity bit is transmitted and checked as a 1<br/>if the EPS bit is 1 then the parity bit is transmitted and checked as a 0. See Table25 9. | RO   | 0x0   |
| 6:5    | WLEN       | Word length. These bits indicate the number of data bits transmitted or received in a frame as follows:<br/>单词长度。这些位表示在一帧中传输或接收的数据位的数量，<br/>b11 = 8 bits<br/>b10 = 7 bits<br/>b01 = 6 bits<br/>b00 = 5 bits. | RO   | 0x0   |
| 4      | FEN        | **Enable FIFOs:**<br/>0 = FIFOs are disabled (character mode)that is, the FIFOs become 1-byte-deep holding registers<br/>1 = transmit and receive FIFO buffers are enabled (FIFO mode).<br/>0 =禁用fifo(字符模式)，即fifo成为1字节深度的持有寄存器<br/>1 =发送和接收FIFO缓冲区被启用(FIFO模式)。 | RW   | 0x0   |
| 3      | STP2       | Two stop bits select. If this bit is set to 1,two stop bits are transmitted at the end of the frame. The receive logic does not check for two stop bits being received.<br/>两个停止位选择。如果该位设置为1，则在帧的末尾传输两个停止位。接收逻辑不检查接收的两个停止位。 | RW   | 0x0   |
| 2      | EPS        | Even parity select. Controls the type of parity the UART uses during transmission and reception:<br/>0 = odd parity. The UART generates or checks for an odd number of 1s in the data and parity bits.<br/>1 = even parity. The UART generates or checks for an even number of 1s in the data and parity bits.<br/>This bit has no effect when the PEN bit disables parity checking and generation.<br/>偶同位选择。控制UART在传输和接收期间使用的奇偶校验类型:<br/>0 =奇数校验。UART生成或检查数据位和奇偶校验位中的奇数个1。<br/>1 =偶数校验。UART生成或检查数据位和奇偶校验位中的偶数个1。<br/>当笔位禁用奇偶校验和生成时，此位没有效果。<br/>See Table 25 9. | RW   | 0x0   |
| 1      | PEN        | Parity enable:奇偶性启用:<br/>0 = parity is disabled and no parity bit added to the data frame<br/>1 = parity checking and generation is<br/>0 =未使能奇偶校验，数据帧未添加奇偶校验位<br/>1 =奇偶校验和生成<br/>enabled. See Table 25 9. | RW   | 0x0   |
| 0      | BRK        | Send break. If this bit is set to 1, a low-level is continually output on the TXD output, after completing transmission of the current character.<br/>发送打破。如果该位设置为1，在完成当前字符的传输后，一个低电平的字符将持续输出到TXD输出上。 |      |       |

### 0x30 CR丨配置（开启关闭串口的功能）

The UART_CR Register is the control register.
NOTE:
To enable transmission, the TXE bit and UARTEN bit must be set to 1.要使能传输，TXE和UARTEN位必须设置为1。

Similarly, to enable reception, the RXE bit and UARTEN bit, must be set to 1.类似地，要启用接收，RXE位和UARTEN位必须设置为1。
NOTE:
Program the control registers as follows:

1. Disable the UART.

2. Wait for the end of transmission or reception of the current character.等待当前字符的传输或接收结束。

3. Flush the transmit FIFO by setting the FEN bit to 0 in the Line Control Register, UART_LCRH.

  通过在线控制寄存器UART_LCRH中设置FEN位为0刷新发送FIFO。

4. Reprogram the Control Register, UART_CR.重新编程控制寄存器UART_CR。

5. Enable the UART.

| Bit(s) | Field Name | Description                                                  | Type | Reset |
| ------ | ---------- | ------------------------------------------------------------ | ---- | ----- |
| 31:16  |            | Reserved - Write as 0, read as don't care                    |      |       |
| 15     | CTSEN      | 设为1，启用CTS硬件流量控制。只有当nUART**CTS**信号被断言时，数据才被传输。 | RW   | 0x0   |
| 14     | RTSEN      | 设为1，开启RTS硬件流量控制。数据只有在接收FIFO中有空间时才被请求。 | RW   | 0x0   |
| 13     | OUT2       | 不支持，写为零，读忽略                                       | RO   | 0x0   |
| 12     | OUT1       | 不支持，写为零，读忽略                                       | RO   | 0x0   |
| 11     | RTS        | 请求发送。这个位是要发送的UART请求的补码，nUARTRTS，调制解调器状态输出。也就是说，当位被编程为1时，nUARTRTS是LOW。 | RW   | 0x0   |
| 10     | DTR        | 不支持，写为零，读忽略                                       | RO   | 0x0   |
| 9      | RXE        | **设为1，接收开启。**                                        | RW   | 0x0   |
| 8      | TXE        | **设为1，发送开启。**                                        | RW   | 0x0   |
| 7      | LBE        | 设为1，回送启用。显示输入内容                                | RW   | 0x0   |
| 6:3    | RTIM       | 不支持，写为零，读忽略                                       | RO   | 0x0   |
| 2      | SIRLP      | 不支持，写为零，读忽略                                       | RO   | 0x0   |
| 1      | SIREN      | 不支持，写为零，读忽略                                       | RO   | 0x0   |
| 0      | UARTEN     | **设为1，启用串口**                                          | RW   | 0x0   |

### 0x34 IFLS丨选择FIFO等级

The UART_IFLS Register is the interrupt FIFO level select register. You can use this register to define the FIFO level that triggers the assertion of the
combined interrupt signal.

UART_IFLS寄存器是中断FIFO级选择寄存器。您可以使用该寄存器定义触发断言的FIFO级别结合中断信号。

The interrupts are generated based on a transition through a level rather than being based on the level. That is, the interrupts are generated when the fill level progresses through the trigger level.

中断是基于通过关卡的转换而不是基于关卡而生成的。也就是说，当填充级别进展到触发级别时，就会产生中断。

The bits are reset so that the trigger level is when the FIFOs are at the half- way mark. 位被重置，当fifo处于中间标记时触发电平。

| Bit(s) | Field Name | Description                                                  | Type | Reset |
| ------ | ---------- | ------------------------------------------------------------ | ---- | ----- |
| 31:12  |            | Reserved - Write as 0, read as don't care                    |      |       |
| 11:9   | RXIFPSEL   | 不支持，写为零，读忽略                                       | RO   | 0x0   |
| 8:6    | TXIFPSEL   | 不支持，写为零，读忽略                                       | RO   | 0x0   |
| 5:3    | RXIFLSEL   | Receive interrupt FIFO level select. The trigger points for the receive interrupt are as follows:<br/>b000 = Receive FIFO becomes 1/8 full<br/>b001 = Receive FIFO becomes 1/4 full<br/>b010 = Receive FIFO becomes 1/2 full<br/>b011 = Receive FIFO becomes 3/4 full<br/>b100 = Receive FIFO becomes 7/8 full<br/>b101-b111 = reserved. | RW   | 0x0   |

### 0x38 IMSC丨中断掩码设置/清除

The UART_IMSC Register is the interrupt mask set/clear register. It is a read/write register. On a read this register returns the current value of the mask on the relevant interrupt. On a write of 1 to the particular bit, it sets the corresponding mask of that interrupt. A write of 0 clears the corresponding mask.

UART_IMSC寄存器是中断掩码设置/清除寄存器。它是一个读/写寄存器。在读取时，该寄存器返回相关中断上掩码的当前值。在对特定位的写操作为1时，它会设置该中断的相应掩码。写为0将清除相应的掩码（**将不再收到相应的中断**）。

| Bit(s) | Field Name | Description                                                  | Type | Reset |
| ------ | ---------- | ------------------------------------------------------------ | ---- | ----- |
| 31:11  |            | Reserved - Write as 0, read as don't care                    |      |       |
| 10     | OEIM       | 溢出错误中断掩码。设为1，产生UART**OE**INTR 中断             | RW   | 0x0   |
| 9      | BEIM       | 中止错误中断掩码。read返回UART**BE**INTR中断的当前掩码。设为1，设置中断掩码。写为0时清除掩码。 | RW   | 0x0   |
| 8      | PEIM       | 奇偶错误中断掩码。read返回UART**PE**INTR中断的当前掩码。设为1，设置中断掩码。写为0时清除掩码。 | RW   | 0x0   |
| 7      | FEIM       | 组帧错误中断掩码。read返回UART**FE**INTR中断的当前掩码。设为1，设置中断掩码。写为0时清除掩码。 | RW   | 0x0   |
| 6      | RTIM       | 接收超时中断掩码。read返回UART**RT**INTR中断的当前掩码。设为1，设置中断掩码。写为0时清除掩码。 | RW   | 0x0   |
| 5      | TXIM       | 发送中断掩码。read返回UART**TX**INTR中断的当前掩码。设为1，设置中断掩码。写为0时清除掩码。 | RW   | 0x0   |
| 4      | RXIM       | Receive interrupt mask. A read returns thecurrent mask for the UARTRXINTRinterrupt. <br/>On a write of 1, the mask of the interrupt is set. A write of 0 clears the mask.<br/>接收中断掩码。read返回UART**RX**INTR中断的当前掩码。设为1，设置中断掩码。写为0时清除掩码。 | RW   | 0x0   |
| 3      | DSRMIM     | 不支持，写为零，读忽略                                       | RW   | 0x0   |
| 2      | DCDMIM     | 不支持，写为零，读忽略                                       | RW   | 0x0   |
| 1      | CTSMIM     | nUART**CTS**模式中断掩码。read返回UART**CTS**INTR中断的当前掩码。设为1，设置中断掩码。写为0时清除。 | RW   | 0x0   |
| 0      | RIMIM      | 不支持，写为零，读忽略                                       | RW   | 0x0   |

### 0x3C RIS丨 原始中断状态

The UART_RIS Register is the raw interrupt status register. It is a read-only register. This register returns the current raw status value, prior to masking, of the corresponding interrupt.
NOTE: All the bits, except for the modem status interrupt bits (bits 3 to 0), are cleared to 0 when reset. The modem status interrupt bits are undefined after reset.

ART_RIS寄存器是原始中断状态寄存器。它是一个只读寄存器。该寄存器返回对应中断的屏蔽之前的当前原始状态值。

注意:所有的位，除了调制解调器状态中断位(位3到0)，重置时都被清除为0。调制解调器状态中断位在复位后是未定义的。

| Bit(s) | Field Name | Description                                                  | Type | Reset |
| ------ | ---------- | ------------------------------------------------------------ | ---- | ----- |
| 31:11  |            | Reserved - Write as 0, read as don't care                    |      |       |
| 10     | OERIS      | 溢出错误中断状态。返回UART**OE**INTR中断的原始中断状态。     | RW   | 0x0   |
| 9      | BERIS      | 中止错误中断状态。返回UART**BE**INTR中断的原始中断状态。     | RW   | 0x0   |
| 8      | PERIS      | 奇偶错误中断状态。返回UART**PE**INTR中断的原始中断状态。     | RW   | 0x0   |
| 7      | FERIS      | 组帧错误中断状态。返回UART**FE**INTR中断的原始中断状态。     | RW   | 0x0   |
| 6      | RTRIS      | 接收超时中断状态。返回UART**RT**INTR中断的原始中断状态。     | RW   | 0x0   |
| 5      | TXRIS      | 发送中断状态。返回UART**TX**INTR中断的原始中断状态。         | RW   | 0x0   |
| 4      | RXRIS      | 接收中断状态。返回UART**RX**INTR中断的原始中断状态。         | RW   | 0x0   |
| 3      | DSRRMIS    | 不支持，写为零，读忽略                                       | RW   | 0x0   |
| 2      | DCDRMIS    | 不支持，写为零，读忽略                                       | RW   | 0x0   |
| 1      | CTSRMIS    | nUART**CTS**模式中断状态。返回UART**CTS**INTR中断的当前状态。 | RW   | 0x0   |
| 0      | RIRMIS     | 不支持，写为零，读忽略                                       |      |       |

### 0x40 MIS丨masked中断状态寄存器

The UART_MIS Register is the masked interrupt status register. This register returns the current masked status value of the corresponding interrupt.
NOTE: All the bits, except for the modem status interrupt bits (bits 3 to 0), are cleared to 0 when reset. The modem status interrupt bits are undefined after reset.

UART_MIS寄存器是屏蔽中断状态寄存器。这个寄存器返回相应中断的当前屏蔽状态值。

注意:所有的位，除了调制解调器状态中断位(位3到0)，重置时都被清除为0。调制解调器状态中断位在复位后是未定义的。

| Bit(s) | Field Name | Description                                                  | Type | Reset |
| ------ | ---------- | ------------------------------------------------------------ | ---- | ----- |
| 31:11  |            | Reserved - Write as 0, read as don't care                    |      |       |
| 10     | OEMIS      | 溢出错误masked中断状态。设为1，产生UART**OE**INTR 中断       | RW   | 0x0   |
| 9      | BEMIS      | 中止错误masked中断状态。返回UART**BE**INTR中断的masked中断状态。 | RW   | 0x0   |
| 8      | PEMIS      | 奇偶错误masked中断状态。read返回UART**PE**INTR中断的masked中断状态。 | RW   | 0x0   |
| 7      | FEMIS      | 组帧错误masked中断状态。read返回UART**FE**INTR中断的masked中断状态。 | RW   | 0x0   |
| 6      | RTMIS      | 接收超时masked中断状态。read返回UART**RT**INTR中断的masked中断状态。 | RW   | 0x0   |
| 5      | TXMIS      | 发送masked中断状态。read返回UART**TX**INTR中断的masked中断状态。 | RW   | 0x0   |
| 4      | RXMIS      | 接收masked中断状态。read返回UART**RX**INTR中断的masked中断状态。 | RW   | 0x0   |
| 3      | DSRMMIS    | 不支持，写为零，读忽略                                       | RW   | 0x0   |
| 2      | DCDMMIS    | 不支持，写为零，读忽略                                       | RW   | 0x0   |
| 1      | CTSMMIS    | nUART**CTS**模式masked中断状态。read返回UART**CTS**INTR中断的masked中断状态。 | RW   | 0x0   |
| 0      | RIMMIS     | 不支持，写为零，读忽略                                       |      |       |

### 0x44 ICR丨中断清除寄存器

The UART_ICR Register is the interrupt clear register.UART_ICR寄存器是中断清除寄存器。

| Bit(s) | Field Name | Description                                                  | Type | Reset |
| ------ | ---------- | ------------------------------------------------------------ | ---- | ----- |
| 31:11  |            | Reserved - Write as 0, read as don't care                    |      |       |
| 10     | OEIC       | Overrun error interrupt clear. Clears the UARTOEINTR interrupt.<br/>溢出错误中断清除。清除UARTOEINTR中断。 | RW   | 0x0   |
| 9      | BEIC       | 中止错误                                                     | RW   | 0x0   |
| 8      | PEIC       | 奇偶错误                                                     | RW   | 0x0   |
| 7      | FEIC       | 组帧错误                                                     | RW   | 0x0   |
| 6      | RTIC       | 接收超时                                                     | RW   | 0x0   |
| 5      | TXIC       | 发送                                                         | RW   | 0x0   |
| 4      | RXIC       | Receive masked interrupt status. Returns the masked interrupt state of the UARTRXINTR interrupt.<br/>接收屏蔽中断状态。返回UART**RX**INTR中断的masked中断状态。 | RW   | 0x0   |
| 3      | DSRMIC     | 不支持，写为零，读忽略                                       | RW   | 0x0   |
| 2      | DCDMIC     | 不支持，写为零，读忽略                                       | RW   | 0x0   |
| 1      | CTSMIC     | nUARTCTS modem masked interrupt status. Returns the masked interrupt state of the UARTCTSINTR interrupt.<br/>nUART**CTS**模式masked中断状态。read返回UART**CTS**INTR中断的masked中断状态。 | RW   | 0x0   |
| 0      | RIMIC      | 不支持，写为零，读忽略                                       |      |       |

```
PUT32(UART0_ICR,0x3ff);     //clear all irqs
```



# 14 Timer (ARM side) 0x3F00B400

## 14.1 Introduction 介绍

The ARM Timer is based on a ARM AP804, but it has a number of differences with the standard ARM

定时器是基于ARM AP804的，但它与标准有很多不同

SP804:

- There is only one timer.只有一个计时器。
- It only runs in continuous mode.它只能在连续模式下运行。
- It has a extra clock pre-divider register.它有一个额外的时钟预分频寄存器。
- It has a extra stop-in-debug-mode control bit.它有一个额外的停止调试模式控制位。
- It also has a 32-bit free running counter.它还有一个32位的自由运行计数器。

The clock from the ARM timer is derived from the system clock. This clock can change dynamically

来自ARM定时器的时钟来源于系统时钟。这个时钟可以动态改变

e.g. if the system goes into reduced power or in low power mode. Thus the clock speed adapts to the overal system performance capabilities. For accurate timing it is recommended to use the system timers.

e.g. 如果系统进入低功率或低功率模式。因此，时钟速度与系统整体性能能力相适应。为了准确计时，建议使用 system timers。

## Timer Registers:

```
#define ARM_TIMER_LOD 0x3F00B400    /*Load*/
#define ARM_TIMER_VAL 0x3F00B404    /*Value (Read Only)*/
#define ARM_TIMER_CTL 0x3F00B408    /*Control*/使能/禁止Timer* 使能输出：是否能产生事件(中断)* 状态：是否能产生了事件(中断)
#define ARM_TIMER_CLI 0x3F00B40C    /*IRQ Clear/Ack(Write only)*/   //计时器清零
#define ARM_TIMER_RIS 0x3F00B410    /*RAW IRQ(Read Only)*/
#define ARM_TIMER_MIS 0x3F00B414    /*Masked IRQ(Read Only)*/
#define ARM_TIMER_RLD 0x3F00B418    /*Reload*/
#define ARM_TIMER_DIV 0x3F00B41C    /*Pre-divider (Not in real 804!)*/
#define ARM_TIMER_CNT 0x3F00B420    /*Free running counter (Not in real 804!)*/     //CNT是定时器的计数器，存储了当前定时器的计数值
```

## 0x400 Load

The timer load register sets the time for the timer to count down. This value is loaded into the timer value register after the load register has been written or if the timer-value register has counted down to 0.

这个0x400寄存器设置Timer倒计时的时间。该值加载到timer value register之后 这个0x400寄存器被写入或者如果0x404已经减到0。

## 0x404 Value (Read Only)

This register holds the current timer value and is counted down when the counter is running. It is counted down each timer clock until the value 0 is reached. Then the value register is re-loaded from the timer load register and the interrupt pending bit is set. The timer count down speed is set by the timer pre-divide register.

该寄存器保存当前计时器值，并在计数器运行时进行计数。它会在每个计时器时钟中倒数，直到达到值0为止。然后从计时器加载寄存器重新加载值寄存器，并设置中断等待位。计时器倒计时速度由timer pre-divide寄存器设置。

```c
STATIC_INLINE UINT64 TimerCvalRead(VOID)
{
    //return READ_TIMER_REG64(TIMER_REG_CVAL);
	return *(volatile UINT32 *)((UINTPTR)IO_DEVICE_ADDR(0x3F00B404));		
}
```

## 0x408 Control

The standard SP804 timer control register consist of 8 bits but in the BCM implementation there are more control bits for the extra features. Control bits 0-7 are identical to the SP804 bits, albeit some functionality of the SP804 is not implemented. All new control bits start from bit 8 upwards. Differences between a real 804 and the BCM implementation are shown in italics.

标准的SP804定时器控制寄存器由8位组成，但是在BCM实现中有更多的控制位用于额外的功能。控制位0-7与SP804位相同，尽管SP804的一些功能没有实现。所有新的控制位从第8位向上开始。实际804和BCM实现之间的区别用斜体显示。

| Name: Timer control | Address: base + 0x408 | Reset: 0x3E0020                                              |
| ------------------- | --------------------- | ------------------------------------------------------------ |
| Bit(s)              | R/W                   | Function                                                     |
| 31:10               | -                     | <Unused>                                                     |
| 23:16               | R/W                   | Free running counter pre-scaler. Freq is sys_clk/(prescale+1)<br/>These bits do not exists in a standard 804! Reset value is 0x3E<br/>自由运行计数器预定标器。sys_clk/(precale +1)<br/>这些位在标准的804中不存在!重置值为0x3E |
| 15:10               | -                     | <Unused>                                                     |
| 9                   | R/W                   | 1 : Free running counter Enabled                             |
| 8                   | R/W                   | 0:如果ARM处于调试暂停模式，计时器将继续运行<br/>1:如果ARM处于调试暂停模式，计时器暂停 |
| 7                   | R/W                   | **1 : Timer enabled**                                        |
| 6                   | R/W                   | Not used                                                     |
| 5                   | R/W                   | **1 : Timer interrupt enabled**                              |
| 4                   | R/W                   | Not used                                                     |
| 3:2                 | R/W                   | Pre-scale bits:<br/>00 : pre-scale is clock / 1 (No pre-scale)<br/>01 : pre-scale is clock / 16<br/>10 : pre-scale is clock / 256<br/>11 : pre-scale is clock / 1 (Undefined in 804) |
| 1                   | R/W                   | 0 : 16-bit counters<br/>1 : 23-bit counter                   |
| 0                   | R/W                   | Not used                                                     |

## 0x40C IRQ Clear/Ack (Write only)

The timer IRQ clear register is write only. When writing this register the interrupt-pending bit is cleared.
When reading this register it returns 0x544D5241 which is the ASCII reversed value for "ARMT".

定时器IRQ清除寄存器是只写的。当写入此寄存器时，中断挂起位将被清除。

当读取该寄存器时，它将返回0x544D5241，这是“ARMT”的ASCII反向值。

```c
*(volatile UINT32 *)((UINTPTR)IO_DEVICE_ADDR(0x3F00B40C)) = 0; //中断挂起位将被清除
```

## 0x410 RAW IRQ (Read Only)

The raw IRQ register is a read-only register. It shows the status of the interrupt pending bit.

原始IRQ寄存器是只读寄存器。它显示了中断挂起位的状态。

```
0 : The interrupt pending bits is clear中断挂起位是清楚的
1 : The interrupt pending bit is set.设置了中断挂起位。
```

The interrupt pending bits is set each time the value register is counted down to zero. The interrupt pending bit can not by itself generates interrupts. Interrupts can only be generated if the interrupt enable bit is set.

每次值寄存器计数为零时，都会设置中断挂起位。中断挂起位本身不能产生中断。只有设置了中断启用位，才能生成中断。

## 0x414 Masked IRQ (Read Only)

The masked IRQ register is a read-only register. It shows the status of the interrupt signal. It is simply a logical AND of the interrupt pending bit and the interrupt enable bit.

被屏蔽的IRQ寄存器是一个只读寄存器。它显示中断信号的状态。它只是一个逻辑和中断等待位和中断启用位。

## 0x418 Reload

This register is a copy of the timer load register. The difference is that a write to this register does not trigger an immediate reload of the timer value register. Instead the timer load register value is only accessed if the value register has finished counting down to zero.

这个寄存器是计时器加载寄存器的副本。不同之处在于，对这个寄存器的写操作不会立即重新加载计时器值寄存器。相反，只有当值寄存器已经完成倒数到零时，才会访问计时器加载寄存器的值。

```c
*(volatile UINT32 *)((UINTPTR)0x3F00B418) = 20000-1; //0x400是设置即生效，0x418当前计时结束再生效
```

## 0x41C Pre-divider (Not in real 804!)

| Name: pre-divide | Address: base + 0x41C | Reset: 0x07D       |
| ---------------- | --------------------- | ------------------ |
| 31:10            |                       | Unused             |
| 9:0              | R/W                   | Pre-divider value. |

The Pre-divider register is not present in the SP804.SP804中不存在预分频寄存器。

The pre-divider register is 10 bits wide and can be written or read from. This register has been added as the SP804 expects a 1MHz clock which we do not have. Instead the pre-divider takes the APB clock and divides it down according to:

预除寄存器是10位宽的，可以写入或读取。这个寄存器已经添加，因为SP804期望一个1MHz时钟，这是我们没有的。相反，预分配器将APB时钟分成以下几个部分:

​		timer_clock = apb_clock/(pre_divider+1)
The reset value of this register is 0x7D so gives a divide by 126.这个寄存器的重置值是0x7D，因此给出一个除以126的值。

## 0x420 Free running counter (Not in real 804!)

The free running counter is not present in the SP804.
The free running counter is a 32 bits wide read only register. The register is enabled by setting bit 9 of the Timer control register. The free running counter is incremented immediately after it is enabled. The timer can not be reset but when enabled, will always increment and roll-over. The free running counter is also running from the APB clock and has its own clock pre-divider controlled by bits 16-23 of the timer control register.

自由运行计数器是一个32位宽的只读寄存器。通过设置定时器控制寄存器的第9位使能该寄存器。空闲运行计数器在启用后立即递增。计时器不能重置，但当启用时，将始终递增和翻转。自由运行计数器也从APB时钟运行，并有自己的时钟预分配器，由16-23位的计时器控制寄存器控制。

This register will be halted too if bit 8 of the control register is set and the ARM is in Debug Halt mode.

如果控制寄存器的第8位被设置并且ARM处于调试暂停模式，这个寄存器也将被停止。