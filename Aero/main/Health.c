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

#include "Health.h"
#define V_REF   1100
//#define ADC1_TEST_CHANNEL (ADC1_CHANNEL_6)      //GPIO 34
#define BATHEALTH (ADC2_CHANNEL_4)      //GPIO 13
#define FILTERHEALTH (ADC2_CHANNEL_3)      //GPIO 15

//#define V_REF_TO_GPIO  //Remove comment on define to route v_ref to GPIO

int Get_Hall()
{
	adc1_config_width(ADC_WIDTH_12Bit);
	return hall_sensor_read();

}

uint32_t Get_BatteryHealth()
{
    //Init ADC and Characteristics
    //esp_adc_cal_characteristics_t characteristics;
    //adc2_config_width(ADC_WIDTH_12Bit);
    adc2_config_channel_atten(BATHEALTH, ADC_ATTEN_11db);
   // esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_11db, ADC_WIDTH_12Bit, &characteristics);
    int voltage;
    uint32_t sum=0;
    for (int i=0;i<5;i++)
    {

    adc2_get_raw( BATHEALTH, ADC_WIDTH_12Bit, &voltage);
    voltage=/*1.294*/1.15*voltage;//3.9/4096*voltage*1.33 ;    //1.36
    vTaskDelay(pdMS_TO_TICKS(50));
    sum+=voltage;
    }
    voltage=sum/5;
       // voltage = adc2_to_voltage(BATHEALTH, &characteristics);
       // printf("%d mV\n",voltage);
        //vTaskDelay(pdMS_TO_TICKS(1000));
        return voltage;

}


uint32_t Get_BatteryHealthInt()
{
	uint32_t voltageInt=0;
    uint32_t batStatx=Get_BatteryHealth();

    if(batStatx>3700)
    	voltageInt=7;
    else if(batStatx>3000 && batStatx<=3700)
    			voltageInt=3;
	else if(batStatx>2000 && batStatx<=3000)
			voltageInt=1;
	else
			voltageInt=0;


     return voltageInt;

}





uint32_t Get_FilterHealth()
{
#ifndef V_REF_TO_GPIO
    //Init ADC and Characteristics
    esp_adc_cal_characteristics_t characteristics;
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten(FILTERHEALTH, ADC_ATTEN_0db);
    esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_0db, ADC_WIDTH_12Bit, &characteristics);
    uint32_t voltage;

        voltage = adc1_to_voltage(BATHEALTH, &characteristics);
       // printf("%d mV\n",voltage);
        //vTaskDelay(pdMS_TO_TICKS(1000));
        return voltage;
#else
    //Get v_ref
    esp_err_t status;
    status = adc2_vref_to_gpio(GPIO_NUM_25);
    if (status == ESP_OK){
        printf("v_ref routed to GPIO\n");
    }else{
        printf("failed to route v_ref\n");
    }
    return 0;
    fflush(stdout);
#endif

}
