/****************************************************************************
 * boards/arm/am335x/beaglebone-black/src/am335x_lcd.c
 *
 *   Copyright (C) 2019 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include <assert.h>
#include <errno.h>
#include <debug.h>

#include "fb.h"

#include "stm32mp157_lcdc.h"
#include "stm32mp157_lcd.h"


static lcd_params lcd_7_0_params = {
	.name = "lcd_7.0",
	.pins_pol = {
		.de    = INVERT,	/* normal: 低电平表示使能输出 */
		.vclk  = INVERT,	/* normal: 在上升降沿获取数据*/
		.hsync = NORMAL,    /* normal: 低脉冲*/
		.vsync = NORMAL, 	/* normal: 低脉冲*/
	},
	.time_seq = {
		/* 垂直方向 */
		.tvp=	3, /* vysnc脉冲宽度 */
		.tvb=	20,  /* 上边黑框, Vertical Back porch */
		.tvf=	12,  /* 下边黑框, Vertical Front porch */

		/* 水平方向 */
		.thp=	20, /* hsync脉冲宽度 */
		.thb=	140,  /* 左边黑框, Horizontal Back porch */
		.thf=	160,  /* 右边黑框, Horizontal Front porch */

		.vclk=	51,  /* MHz */
	},
	.xres = 1024,
	.yres = 600,
	.bpp  = 16,       
	.fb_base = LCD_FB_BASE,
	.fb_vbase = LCD_FB_VBASE,
};


int up_fbinitialize(int display)
{
	PRINT_RELEASE("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	Imx6ull_lcd_controller_init(&lcd_7_0_params);
	PRINT_RELEASE("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

