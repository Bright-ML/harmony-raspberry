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

# 查询

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

# 开启

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

# 关闭

## 0x21C Disable IRQs 1

## 0x220 Disable IRQs 2

## 0x224 Disable Basic IRQs