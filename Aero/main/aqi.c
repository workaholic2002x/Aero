/*
 * aqi.c
 *
 *  Created on: Jan 11, 2018
 *      Author: cmaity
 */

#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_system.h"
#include "esp_adc_cal.h"
#include "driver/adc.h"

#include "aqi.h"
#include "math.h"
#define V_REFDUST   1100

uint32_t Dust_Density;
uint32_t Dust_DensityMin=999;
uint32_t Dust_DensityMax=0;

uint32_t Get_DustSensorLocal();

static void dust_task(void* arg)
{
//int i=0;
    for(;;)
    {

    	Dust_Density=(Dust_Density+Get_DustSensorLocal())/2;//Get_DustSensor();
    	vTaskDelay(250/portTICK_RATE_MS);
    }

}




void init_DustSensor()
{

	//Init LED as Output
 	gpio_pad_select_gpio(AQI_LED);
	gpio_set_direction(AQI_LED, GPIO_MODE_OUTPUT);
	gpio_pullup_dis(AQI_LED);
	gpio_pulldown_en(AQI_LED);

	/*
	//Init Analog In as sensing
	gpio_pad_select_gpio(LED2_OUT);
	gpio_set_direction(LED2_OUT, GPIO_MODE_OUTPUT);
	gpio_pullup_dis(LED2_OUT);
	gpio_pulldown_en(LED2_OUT);

	gpio_pad_select_gpio(LED3_OUT);
	gpio_set_direction(LED3_OUT, GPIO_MODE_OUTPUT);
	gpio_pullup_dis(LED3_OUT);
	gpio_pulldown_en(LED3_OUT);
	*/
	xTaskCreate(&dust_task, "dust_task", 2048,NULL,10,NULL );
}


uint32_t Get_DustSensorLocal()
{

	double _aqi=0;
    esp_adc_cal_characteristics_t characteristics;
    adc1_config_width(ADC_WIDTH_10Bit);
    adc1_config_channel_atten(AQI_IN, ADC_ATTEN_0db);
    esp_adc_cal_get_characteristics(V_REFDUST, ADC_ATTEN_0db, ADC_WIDTH_10Bit, &characteristics);
    double voltage;
    double sum=0;//Dust_Density;//0;
    for (int il=0;il<100;il++)
       {
    	//LED ON
    	gpio_set_level(AQI_LED, 0);
    	ets_delay_us(280);

        voltage = adc1_to_voltage(AQI_IN, &characteristics);
        //vTaskDelay(pdMS_TO_TICKS(50));
        ets_delay_us(60); //60<-40
        //LED OFF
        gpio_set_level(AQI_LED, 1);
        //voltage=0.171*voltage-0.1;

        sum+=voltage*voltage;

        //ets_delay_us(9680);
        vTaskDelay(30/portTICK_RATE_MS);
       }
    voltage=sqrt(sum/100);

    //raw voltage
    if(voltage<300 && voltage>=250)
    {
    	_aqi=(voltage-250)/10*5;
    }
    else  if(voltage<=500 && voltage>=300)
    {
    	_aqi=(voltage-300)+30;
    }
    else  if(voltage>500)
       {
       	_aqi=(voltage-500)+230;
       }


//    if(Dust_Density>0)
//    voltage=(voltage+Dust_Density)/2;

/*
    if(voltage<=15.4)
    {
    	_aqi=0+(voltage-0)*(50-0)/(15.4-0);
    }
    else if(voltage>15.4 && voltage <=40.4)
    {
    	_aqi=51+(voltage-15.5)*(100-51)/(40.4-15.5);
    }
    else if(voltage>40.4 && voltage <=65.4)
       {
       	_aqi=101+(voltage-40.5)*(150-101)/(65.4-40.5);
       }
    else if(voltage>65.4 && voltage <=150.4)
           {
           	_aqi=151+(voltage-65.5)*(200-151)/(150.4-65.5);
           }
    else if(voltage>150.4 && voltage <=250.4)
           {
           	_aqi=201+(voltage-150.5)*(300-201)/(250.4-150.5);
           }
    else if(voltage>250.4 && voltage <=500.4)
               {
               	_aqi=301+(voltage-250.5)*(500-301)/(500.4-250.5);
               }
    else
    	_aqi=0;
*/

/*
    //link: https://github.com/vlytsus/arduinosensor
    if(voltage<=35)
      {
      	_aqi=0+(voltage-0)*(50-0)/(35-0);
      }
      else if(voltage>35 && voltage <=75)
      {
      	_aqi=51+(voltage-36)*(100-51)/(75-36);
      }
      else if(voltage>75 && voltage <=115)
         {
         	_aqi=101+(voltage-76)*(150-101)/(115-76);
         }
      else if(voltage>115 && voltage <=150)
             {
             	_aqi=151+(voltage-116)*(200-151)/(150-116);
             }
      else if(voltage>150 && voltage <=250)
             {
             	_aqi=201+(voltage-151)*(300-201)/(250-151);
             }
      else if(voltage>250 && voltage <=500)
                 {
                 	_aqi=301+(voltage-251)*(500-301)/(500-251);
                 }
      else
      	_aqi=0;

       // printf("%d mV\n",voltage);
        //vTaskDelay(pdMS_TO_TICKS(1000));
  */
        return (uint32_t)_aqi;//voltage;

}

uint32_t Get_DustSensor()
{
	if(Dust_DensityMin>Dust_Density)
		Dust_DensityMin=Dust_Density;
	if(Dust_Density>Dust_DensityMax)
		Dust_DensityMax=Dust_Density;
	return Dust_Density;
}

uint32_t Get_DustSensorMin()
{
	return Dust_DensityMin;
}

uint32_t Get_DustSensorMax()
{
	return Dust_DensityMax;
}

void Set_DustSensorMinMax()
{
	Dust_DensityMin=999;
	Dust_DensityMax=0;
}

