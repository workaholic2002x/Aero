/*
 * fan.c
 *
 *  Created on: Nov 3, 2017
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
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp32/ulp.h"
#include "soc/soc_ulp.h"
//#include "ulp_main.h"


#include "soc/rtc_io_reg.h"
#include "soc/soc.h"


#include <stdio.h>
//#include "esp_deep_sleep.h"
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/sens_reg.h"
#include "soc/soc.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp32/ulp.h"
//#include "ulp_main.h"


#include "fan.h"
#include "Health.h"


static int fan_state=0;
TaskHandle_t * const GFANTask;

void FanSpeed(void *pvParameter)
{

    while(1) {

    	if(fan_state==0)
    	{
    		gpio_set_level(FAN_OUTPUT, 0);
    		vTaskDelete( GFANTask );
    		//vTaskDelay(100 / portTICK_RATE_MS);
    	}
    	else if(fan_state==1)
    	{
    		gpio_set_level(FAN_OUTPUT, 1);
    		vTaskDelay(100 / portTICK_RATE_MS);
    		gpio_set_level(FAN_OUTPUT, 0);
    		vTaskDelay(200 / portTICK_RATE_MS);
    	}
    	else if(fan_state==2)
    	{
    		gpio_set_level(FAN_OUTPUT, 1);
    		vTaskDelay(200 / portTICK_RATE_MS);
    		gpio_set_level(FAN_OUTPUT, 0);
    		vTaskDelay(100 / portTICK_RATE_MS);
    	}
    	else if(fan_state==3)
    	{
    		gpio_set_level(FAN_OUTPUT, 1);
    		//vTaskDelay(100 / portTICK_RATE_MS);
    		vTaskDelete( GFANTask );
    	}



    //    printf("Hello Shellios:3! ############\n");
    }
}

void init_fan()
{
	  gpio_pad_select_gpio(FAN_OUTPUT);
	    /* Set the GPIO as a push/pull output */
	    gpio_set_direction(FAN_OUTPUT, GPIO_MODE_OUTPUT);
	    gpio_pullup_dis(FAN_OUTPUT);
	    gpio_pulldown_en(FAN_OUTPUT);


	   // xTaskCreate(&FanSpeed, "fan_speed", 2048,NULL,5,NULL );

}



void set_fan (int state)
{
	fan_state=state;
	printf("\n Fan Status=%d\n", state);
	xTaskCreate(&FanSpeed, "fan_speed", 2048,NULL,10,GFANTask );
}


int get_fan()
{
return fan_state;
}


