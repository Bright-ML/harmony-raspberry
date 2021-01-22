# 14 Timer (ARM side)

Introduction 介绍
The ARM Timer is based on a ARM AP804, but it has a number of differences with the standard ARM定时器是基于ARM AP804的，但它与标准有很多不同
SP804:
•There is only one timer.只有一个计时器。
•It only runs in continuous mode.它只能在连续模式下运行。
•It has a extra clock pre-divider register.它有一个额外的时钟预分频寄存器。
•It has a extra stop-in-debug-mode control bit.它有一个额外的停止调试模式控制位。
•It also has a 32-bit free running counter.它还有一个32位的免费跑步计数器。

The clock from the ARM timer is derived from the system clock. This clock can change dynamically
e.g. if the system goes into reduced power or in low power mode. Thus the clock speed adapts to the overal system performance capabilities. For accurate timing it is recommended to use the system timers.

来自ARM定时器的时钟来源于系统时钟。这个时钟可以动态改变

e.g. 如果系统进入低功率或低功率模式。因此，时钟速度与系统整体性能能力相适应。为了准确计时，建议使用系统定时器。

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