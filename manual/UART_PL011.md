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

## UART Interrupts

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

UART RX INTR:

The transmit interrupt changes state when one of the following events occurs:当下列事件之一发生时，发送中断改变状态:

​		• If the FIFOs are enabled and the transmit FIFO is equal to or lower than the programmed trigger level then the transmit interrupt is asserted HIGH. The transmit interrupt is cleared by writing data to the transmit FIFO until it becomes greater than the trigger level, or by clearing the interrupt.

如果FIFO被启用并且发送FIFO等于或低于程序触发水平，那么发送中断被断言为高。发送中断通过写数据到发送FIFO被清除，直到它变得大于触发级别，或者通过清除中断。

​		• If the FIFOs are disabled (have a depth of one location) and there is no data present in the transmitters single location, the transmit interrupt is asserted HIGH. It is cleared by performing a single write to the transmit FIFO, or by clearing the interrupt.

如果fifo被禁用(有一个位置的深度)并且在发射机的单个位置没有数据存在，发射中断被断言为高。它通过对发送FIFO执行一次写操作或清除中断来清除。

UART RT INTR:

The receive interrupt changes state when one of the following events occurs:当下列事件之一发生时，接收中断改变状态:

​		• If the FIFOs are enabled and the receive FIFO reaches the programmed trigger level. When this happens, the receive interrupt is asserted HIGH. The receive interrupt is cleared by reading data from the receive FIFO until it becomes less than the trigger level, or by clearing the interrupt.

如果FIFO被启用并且接收FIFO达到编程的触发电平。当这种情况发生时，接收中断被断言为高中断。接收中断通过从接收的FIFO读取数据来清除，直到它变得小于触发级别，或者通过清除中断。

​		• If the FIFOs are disabled (have a depth of one location) and data is received thereby filling the location, the receive interrupt is asserted HIGH. The receive interrupt is cleared by performing a single read of the receive FIFO, or by clearing the interrupt.

如果fifo被禁用(有一个位置的深度)并且数据被接收从而填充位置，接收中断被断言为高。接收中断被清除通过执行一个单独的读取接收FIFO，或清除中断。

## Register View

The PL011 USRT is mapped on base adderss 0x7E20100. It has the following memory- mapped registers.

```
//#define UART0_BASE 0x20000000 /* "BCM2835.h"  Original B,A,A+,B+ */
#define UART0_BASE 0x3F000000 /* "BCM2836.h"  Raspberriy Pi 2 */

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

```
For words to be transmitted:
if the FIFOs are enabled, data written to this location is pushed onto the transmit FIFO.
if the FIFOs are not enabled, data is stored in the transmitter holding register(the bottom word of the transmit FIFO).
The write operation initiates transmission from the UART. The data is prefixed with a start bit, appended with the appropriate parity bit (if parity is enabled), and a stop bit. The resultant word is then transmitted.
对于要传输的字:如果启用了FIFO，写入到此位置的数据将被推入传输FIFO。如果FIFO未启用，数据将存储在发送器保存寄存器(发送FIFO的底部字)中。
写操作从UART开始传输。数据以开始位为前缀，附加适当的奇偶校验位(如果启用了奇偶校验)和停止位。由此产生的文字就被传送出去。
For received words:
if the FIFOs are enabled, the data byte and the 4-bit status (break, frame,parity, and overrun) is pushed onto the 12-bit wide receive FIFO if the FIFOs are not enabled, the data byte and status are stored in the receiving holding register (the bottom word of the receive FIFO).
收到的话:如果启用了FIFO,数据字节,4比特状态(休息,框架、平价和溢出)推到12位宽接收FIFO如果FIFO不启用,字节的数据和状态存储在接收存储寄存器(接收FIFO的底部)。
```



### 0x18 FR丨标记位

| Bit(s) | Field Name | Description                                                  | Type | Reset |
| ------ | ---------- | ------------------------------------------------------------ | ---- | ----- |
| 31:9   |            | 保留-写为0，读忽略                                           |      |       |
| 8      | RI         | 不支持，写为零，读忽略                                       | RW   | 0x0   |
| 7      | TXFE       | Transmit FIFO empty. The meaning of this bit depends on the state of the FEN bit in the Line Control Register, UARTLCR_LCRH.<br/>If the FIFO is disabled, this bit is set when the transmit holding register is empty.<br/>If the FIFO is enabled, the TXFE bit is set when the transmit FIFO is empty. This bit does not indicate if there is data in the transmit shift register<br/>FIFO启用，**1:transmit FIFO空**；FIFO禁用，0:transmit holding register空。 | RW   | 0x0   |
| 6      | RXFF       | Receive FIFO full. The meaning of this bit depends on the state of the FEN bit in the UARTLCR_ LCRH Register.<br/>If the FIFO is disabled, this bit is set when the receive holding register is full.<br/>If the FIFO is enabled, the RXFF bit is set when the receive FIFO is full.<br/>FIFO启用，**1:receive FIFO满**；FIFO禁用，0:receive holding register满。 | RO   | 0x0   |
| 5      | TXFF       | Transmit FIFO full. The meaning of this bit depends on the state of the FEN bit in the UARTLCR_ LCRH Register.<br/>If the FIFO is disabled, this bit is set when the transmit holding register is full.<br/>If the FIFO is enabled, the TXFF bit is set when the transmit FIFO is full.<br/>FIFO启用，**1:transmit FIFO满**；FIFO禁用，0:transmit holding register满。 | RO   | 0x0   |
| 4      | RXFE       | Receive FIFO empty. The meaning of this bit depends on the state of the FEN bit in the UARTLCR_H Register.<br/>If the FIFO is disabled, this bit is set when the receive holding register is empty.<br/>If the FIFO is enabled, the RXFE bit is set when the receive FIFO is empty.<br/>FIFO启用，**1:Receive FIFO空**；FIFO禁用，0:receive holding register空。 | RW   | 0x0   |
| 3      | BUSY       | UART busy. If this bit is set to 1, the UART is busy transmitting data. This bit remains set until the complete byte, including all the stop bits, has been sent from the **shift register**.<br/>This bit is set as soon as the transmit FIFO becomes non-empty, regardless of whether the UART is enabled or not.<br/>1:表示UART正在忙着传输数据。该位保持设置，直到包括所有停止位在内的完整字节从移位寄存器发出为止。<br/>只要transmit FIFO有值，该位=1，而不管UART是否启用 | RW   | 0x0   |
| 2      | DCD        | 不支持，写为零，读忽略                                       | RW   | 0x0   |
| 1      | DSR        | 不支持，写为零，读忽略。                                     | RW   | 0x0   |

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
| 4      | FEN        | Enable FIFOs:<br/>0 = FIFOs are disabled (character mode)that is, the FIFOs become 1-byte-deep holding registers<br/>1 = transmit and receive FIFO buffers are enabled (FIFO mode).<br/>0 =禁用fifo(字符模式)，即fifo成为1字节深度的持有寄存器<br/>1 =发送和接收FIFO缓冲区被启用(FIFO模式)。 | RW   | 0x0   |
| 3      | STP2       | Two stop bits select. If this bit is set to 1,two stop bits are transmitted at the end of the frame. The receive logic does not check for two stop bits being received.<br/>两个停止位选择。如果该位设置为1，则在帧的末尾传输两个停止位。接收逻辑不检查接收的两个停止位。 | RW   | 0x0   |
| 2      | EPS        | Even parity select. Controls the type of parity the UART uses during transmission and reception:<br/>0 = odd parity. The UART generates or checks for an odd number of 1s in the data and parity bits.<br/>1 = even parity. The UART generates or checks for an even number of 1s in the data and parity bits.<br/>This bit has no effect when the PEN bit disables parity checking and generation.<br/>偶同位选择。控制UART在传输和接收期间使用的奇偶校验类型:<br/>0 =奇数校验。UART生成或检查数据位和奇偶校验位中的奇数个1。<br/>1 =偶数校验。UART生成或检查数据位和奇偶校验位中的偶数个1。<br/>当笔位禁用奇偶校验和生成时，此位没有效果。<br/>See Table 25 9. | RW   | 0x0   |
| 1      | PEN        | Parity enable:奇偶性启用:<br/>0 = parity is disabled and no parity bit added to the data frame<br/>1 = parity checking and generation is<br/>0 =未使能奇偶校验，数据帧未添加奇偶校验位<br/>1 =奇偶校验和生成<br/>enabled. See Table 25 9. | RW   | 0x0   |
| 0      | BRK        | Send break. If this bit is set to 1, a low-level is continually output on the TXD output, after completing transmission of the current character.<br/>发送打破。如果该位设置为1，在完成当前字符的传输后，一个低电平的字符将持续输出到TXD输出上。 |      |       |

### 0x30 CR丨配置

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
| 9      | RXE        | 设为1，接收开启。                                            | RW   | 0x0   |
| 8      | TXE        | 设为1，发送开启。                                            | RW   | 0x0   |
| 7      | LBE        | 设为1，回送启用。显示输入内容                                | RW   | 0x0   |
| 6:3    | RTIM       | 不支持，写为零，读忽略                                       | RO   | 0x0   |
| 2      | SIRLP      | 不支持，写为零，读忽略                                       | RO   | 0x0   |
| 1      | SIREN      | 不支持，写为零，读忽略                                       | RO   | 0x0   |
| 0      | UARTEN     | 设为1，启用串口                                              | RW   | 0x0   |

### 0x34 IFLS丨选择FIFO等级

The UART_IFLS Register is the interrupt FIFO level select register. You can use this register to define the FIFO level that triggers the assertion of the
combined interrupt signal.

UART_IFLS寄存器是中断FIFO级选择寄存器。您可以使用该寄存器定义触发断言的FIFO级别结合中断信号。

The interrupts are generated based on a transition through a level rather than being based on the level. That is, the interrupts are generated when the fill level progresses through the trigger level.

中断是基于通过关卡的转换而不是基于关卡而生成的。也就是说，当填充级别进展到触发级别时，就会产生中断。

The bits are reset so that the trigger level is when the FIFOs are at the half- way mark. 位被重置，当fifo处于中间标记时触发电平。

### 0x38 IMSC丨中断掩码设置/清除

The UART_IMSC Register is the interrupt mask set/clear register. It is a read/write register. On a read this register returns the current value of the mask on the relevant interrupt. On a write of 1 to the particular bit, it sets the corresponding mask of that interrupt. A write of 0 clears the corresponding mask.

UART_IMSC寄存器是中断掩码设置/清除寄存器。它是一个读/写寄存器。在读取时，该寄存器返回相关中断上掩码的当前值。在对特定位的写操作为1时，它会设置该中断的相应掩码。写为0将清除相应的掩码。

| Bit(s) | Field Name | Description                                                  | Type | Reset |
| ------ | ---------- | ------------------------------------------------------------ | ---- | ----- |
| 31:11  |            | Reserved - Write as 0, read as don't care                    |      |       |
| 10     | OEIM       | 溢出错误中断掩码。设为1，产生UART**OE**INTR 中断             | RW   | 0x0   |
| 9      | BEIM       | 中止错误中断掩码。read返回UART**BE**INTR中断的当前掩码。设为1，设置中断掩码。写为0时清除掩码。 | RW   | 0x0   |
| 8      | PEIM       | 奇偶错误中断掩码。read返回UART**PE**INTR中断的当前掩码。设为1，设置中断掩码。写为0时清除掩码。 | RW   | 0x0   |
| 7      | FEIM       | 组帧错误中断掩码。read返回UART**FE**INTR中断的当前掩码。设为1，设置中断掩码。写为0时清除掩码。 | RW   | 0x0   |
| 6      | RTIM       | 接收超时中断掩码。read返回UART**RT**INTR中断的当前掩码。设为1，设置中断掩码。写为0时清除掩码。 | RW   | 0x0   |
| 5      | TXIM       | 发送中断掩码。read返回UART**TX**INTR中断的当前掩码。设为1，设置中断掩码。写为0时清除掩码。 | RW   | 0x0   |
| 4      | RXIM       | 接收中断掩码。read返回UART**RX**INTR中断的当前掩码。设为1，设置中断掩码。写为0时清除掩码。 | RW   | 0x0   |
| 3      | DSRMIM     | 不支持，写为零，读忽略                                       | RW   | 0x0   |
| 2      | DCDMIM     | 不支持，写为零，读忽略                                       | RW   | 0x0   |
| 1      | CTSMIM     | nUART**CTS**模式中断掩码。read返回UART**CTS**INTR中断的当前掩码。设为1，设置中断掩码。写为0时清除。 | RW   | 0x0   |
| 0      | RIMIM      | 不支持，写为零，读忽略                                       | RW   | 0x0   |



### 0x44 ICR丨中断清除寄存器

```
PUT32(UART0_ICR,0x3ff);     //clear all irqs
```

