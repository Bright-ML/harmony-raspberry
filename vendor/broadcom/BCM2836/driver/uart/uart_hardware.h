/*
 * Copyright (c) 2013-2019, Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020, Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _AMBA_PL011_UART_H
#define _AMBA_PL011_UART_H

#include "los_typedef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define UART_DR                 0x0   /* data register */
#define UART_RSR                0x04
#define UART_FR                 0x18  /* flag register */
#define UART_CLR                0x44  /* interrupt clear register */
#define UART_CR                 0x30  /* control register */
#define UART_IBRD               0x24  /* interge baudrate register */
#define UART_FBRD               0x28  /* decimal baudrate register */
#define UART_LCR_H              0x2C
#define UART_IFLS               0x34  /* fifo register */
#define UART_IMSC               0x38  /* interrupt mask register */
#define UART_RIS                0x3C  /* base interrupt state register */
#define UART_MIS                0x40  /* mask interrupt state register */
#define UART_ICR                0x44
#define UART_DMACR              0x48  /* DMA control register */

/*---增加的代码---*/
/*根据STM32MP157芯片手册<<53.7 Universal synchronous/asynchronous receiver transmitter (USART/UART)>>的2655页，定义USART4的结构体,*/
typedef struct {
  volatile unsigned int  USART_CR1;          /**< USART control register 1,                 offset: 0x00 	 串口控制寄存器1，            偏移地址0x00  */
  volatile unsigned int  USART_CR2;          /**< USART control register 2,                 offset: 0x04 	 串口控制寄存器2，            偏移地址0x04  */
  volatile unsigned int  USART_CR3;          /**< USART control register 3,                 offset: 0x08 	 串口控制寄存器3，            偏移地址0x08  */
  volatile unsigned int  USART_BRR;          /**< USART Baud Rate register,                 offset: 0x0C     串口波特率寄存器             偏移地址0x0C  */
  volatile unsigned int  USART_GTPR;         /**< USART guard time and prescaler register,  offset: 0x10     串口保护时间和预分频器寄存器 偏移地址0x10  */
  volatile unsigned int  USART_RTOR;         /**< USART receiver timeout register,          offset: 0x14     串口接收超时寄存器           偏移地址0x14  */
  volatile unsigned int  USART_RQR;          /**< USART request register,                   offset: 0x18     串口请求寄存器               偏移地址0x18  */
  volatile unsigned int  USART_ISR;          /**< USART interrupt and status register,      offset: 0x1C     串口中断与状态寄存器         偏移地址0x1C  */
  volatile unsigned int  USART_ICR;          /**< USART interrupt flag clear register ,     offset: 0x20     串口中断状态清除寄存器       偏移地址0x20  */
  volatile unsigned int  USART_RDR;          /**< USART receive data register,              offset: 0x24     串口接收数据寄存器           偏移地址0x24  */
  volatile unsigned int  USART_TDR;          /**< USART transmit data register,             offset: 0x28     串口发送数据寄存器           偏移地址0x28  */
  volatile unsigned int  USART_PRESC;        /**< USART prescaler register,                 offset: 0x2C     串口预分频器寄存器           偏移地址0x2C  */
} UART_Type;


/*UART1的寄存器的基地址*/
//#define UART1_BASE          (0x2020000u)

#define STM32MP157_UART4      ((UART_Type *)UART4_REG_BASE)
#define STM32MP157_PHY_UART4  ((UART_Type *)UART4_REG_PBASE)
/*----------------*/

#define CMD_LENGTH  128

extern CHAR g_inputCmd[CMD_LENGTH];
extern INT32 g_inputIdx;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif
