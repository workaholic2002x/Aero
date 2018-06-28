/* ULP Example: testing if RTCIO signals may be retained during ESP32 hibernation

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   This file contains assembly code which runs on the ULP.
*/

/* ULP assembly files are passed through C preprocessor first, so include directives
   and C macros may be used in these files 
 */
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/soc_ulp.h"

	/* Define variables, which go into .bss section (zero-initialized data) */
	.bss

	.global toggle_counter
toggle_counter:
	.long 0

	/* Number of restarts of ULP to wake up the main program.
	   See couple of lines below how this value is used */
	.set toggle_cycles_to_wakeup, 0b1000000000000111

	/* Code goes into .text section */
	.text
	.global entry
entry:
	/* Disable hold of  RTC_GPIO15, GPIO12, Touch5 output */
	//WRITE_RTC_REG(RTC_IO_TOUCH_PAD5_REG,RTC_IO_TOUCH_PAD5_HOLD_S,1,0)
	
	/* Set the RTC_GPIO15(IO12) output HIGH
	   to signal that ULP is now up */
	//WRITE_RTC_REG(RTC_GPIO_OUT_W1TS_REG,RTC_GPIO_OUT_DATA_W1TS_S+15,1,1)
	
	/* Enable hold on RTC_GPIO15(IO12) output */
	//WRITE_RTC_REG(RTC_IO_TOUCH_PAD5_REG,RTC_IO_TOUCH_PAD5_HOLD_S,1,1)
	
	/* Wait some cycles to have visible trace on the scope */
	//wait 10000000	

	/* Disable hold of  RTC_GPIO15, GPIO12, Touch5 output */
	//WRITE_RTC_REG(RTC_IO_TOUCH_PAD5_REG,RTC_IO_TOUCH_PAD5_HOLD_S,1,0)
	
	WRITE_RTC_REG(RTC_GPIO_OUT_REG, RTC_GPIO_OUT_DATA_S + 15, 1, 1)
	
	
	/*cm Set the RTC_GPIO15(IO12) output LOW (clear output)
	   to signal that ULP is now going down */
	//WRITE_RTC_REG(RTC_GPIO_OUT_W1TC_REG,RTC_GPIO_OUT_DATA_W1TC_S+15,1,1)
	
	/* Enable hold on RTC_GPIO15(IO12) output */
	//WRITE_RTC_REG(RTC_IO_TOUCH_PAD5_REG,RTC_IO_TOUCH_PAD5_HOLD_S,1,1)
	
	wait 1000000	



	
	jump entry
	
	
	