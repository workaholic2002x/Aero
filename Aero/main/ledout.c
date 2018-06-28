/*
 * ledout.c
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
#include "stdbool.h"

#include "ledout.h"

TaskHandle_t * const GLEDTask;
static int LedNo=0;
static int Duration=0;
static bool BLINK=false;
static int BLINK_FLAG=0;
static int BlinkRate=1;

void set_gpio(int no,int state)
{

	if(no==1)
	{

		gpio_set_level(LED1_OUT, state);
		gpio_set_level(LED2_OUT, 0);
		gpio_set_level(LED3_OUT, 0);
	}
	if(no==2)
		{
			gpio_set_level(LED1_OUT, 0);
			gpio_set_level(LED2_OUT, state);
			gpio_set_level(LED3_OUT, 0);
		}
	if(no==3)
		{
			gpio_set_level(LED1_OUT, state);
			gpio_set_level(LED2_OUT, state);
			gpio_set_level(LED3_OUT, 0);
		}
	if(no==4)
		{
			gpio_set_level(LED1_OUT, 0);
			gpio_set_level(LED2_OUT, 0);
			gpio_set_level(LED3_OUT, state);
		}
	if(no==5)
		{
			gpio_set_level(LED1_OUT, state);
			gpio_set_level(LED2_OUT, 0);
			gpio_set_level(LED3_OUT, state);
		}
	if(no==6)
		{
			gpio_set_level(LED1_OUT, 0);
			gpio_set_level(LED2_OUT, state);
			gpio_set_level(LED3_OUT, state);
		}
	if(no==7)
		{
			gpio_set_level(LED1_OUT, state);
			gpio_set_level(LED2_OUT, state);
			gpio_set_level(LED3_OUT, state);
		}
}


static void led_task(void* arg)
{

    for(;;)
    {
    		if(Duration>=0)
    		{

    			if(BLINK==false)
    				set_gpio(LedNo, 1);
    			else
    			{
    				if(BLINK_FLAG==0)
    				   BLINK_FLAG=1;
    				else
    					BLINK_FLAG=0;

    				set_gpio(LedNo, BLINK_FLAG);

    			}
    	      Duration--;
    		}
    		else
    		{
    			if(LedNo>0)
    			{
    			set_gpio(LedNo, 0);
    			LedNo=0;
    			BlinkRate=1;
    			}
    			vTaskDelete( GLEDTask );
    		}



    	 vTaskDelay(1000/(portTICK_RATE_MS*BlinkRate));
    }


}

void init_LED()
{

 	gpio_pad_select_gpio(LED1_OUT);
	gpio_set_direction(LED1_OUT, GPIO_MODE_OUTPUT);
	gpio_pullup_dis(LED1_OUT);
	gpio_pulldown_en(LED1_OUT);

	gpio_pad_select_gpio(LED2_OUT);
	gpio_set_direction(LED2_OUT, GPIO_MODE_OUTPUT);
	gpio_pullup_dis(LED2_OUT);
	gpio_pulldown_en(LED2_OUT);

	gpio_pad_select_gpio(LED3_OUT);
	gpio_set_direction(LED3_OUT, GPIO_MODE_OUTPUT);
	gpio_pullup_dis(LED3_OUT);
	gpio_pulldown_en(LED3_OUT);

	//xTaskCreate(&led_task, "led_task", 2048,NULL,10,NULL );
}

void Set_LED(int _LedNo, int _DurSec, int _BlinkRateHz)
{
	if(_LedNo>7)
		_LedNo=_LedNo%7;
	if(_DurSec>100) //max 10 sec
		_DurSec=100;

	if(_BlinkRateHz>25)
		_BlinkRateHz=25;

	BLINK=false;
	if(_BlinkRateHz>1)
{
		BLINK=true;
	BlinkRate=_BlinkRateHz;
}
else
	BlinkRate=1;

LedNo=_LedNo;
Duration=_DurSec*BlinkRate;
if(Duration==3)
	Duration++;
xTaskCreate(&led_task, "led_task", 2048,NULL,10,GLEDTask );
}

int Get_LED()
{
int state=0;
if(gpio_get_level(LED1_OUT))
	state=1;
if(gpio_get_level(LED2_OUT))
	state=state<<1;
if(gpio_get_level(LED3_OUT))
	state=state<<2;
return state;

}
