/*
 * buzzer.c
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

#include "buzzer.h"

int Duration=0;
int Count=0;
TaskHandle_t * const GBUZZERTask;


static void buzzer_task(void* arg)
{

    for(;;)
    {
    		if(Count>0)
    		{
    			gpio_set_level(BUZZER_OUT, 1);
    			 vTaskDelay(Duration/portTICK_RATE_MS);
    			 gpio_set_level(BUZZER_OUT, 0);
    			 vTaskDelay(Duration/portTICK_RATE_MS);


    	      Count--;
    		}
    		else
    		{
    			gpio_set_level(BUZZER_OUT, 0);
    			vTaskDelete( GBUZZERTask );
    		}


    }
}


void init_BUZZER()
{

 	gpio_pad_select_gpio(BUZZER_OUT);
	gpio_set_direction(BUZZER_OUT, GPIO_MODE_OUTPUT);
	gpio_pullup_dis(BUZZER_OUT);
	gpio_pulldown_en(BUZZER_OUT);


	//xTaskCreate(&buzzer_task, "led_task", 2048,NULL,10,NULL );
}

void Set_BUZZER(int _duration,int _count)
{
if(_duration>1000)
	_duration=1000;
if(_duration<50)
	_duration=50;
Duration=_duration;

if(_count>10) //10
	_count=10;  //10
Count=_count;

if(Count>0)
xTaskCreate(&buzzer_task, "buzzer_task", 2048,NULL,10,GBUZZERTask );
}
