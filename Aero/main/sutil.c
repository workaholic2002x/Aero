/*
 * util.c
 *
 *  Created on: Nov 5, 2017
 *      Author: cmaity
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/gpio.h"
#include "esp_sleep.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/soc.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"

#include "esp_deep_sleep.h"
#include "esp32/ulp.h"
//#include "ulp_main.h"

#include "time.h"

#include "charging.h"


#include "sutil.h"

static unsigned long int RefTime=0;


long get_Epoch()
{
	time_t DTnow;
	time(&DTnow);
	//long t_of_day=mktime(&DTnow);

	return DTnow;//t_of_day;
}
