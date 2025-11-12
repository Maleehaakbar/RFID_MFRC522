#include "iot_servo.h"
#include "esp_log.h"
#include "servo.h"
#include "freertos/FreeRTOS.h"

#define SERVO_PIN  6

const char* TAG_SERVO = "servo_app";
void servo_on();
void servo_init()
{   
    servo_config_t servo_cfg = {
        .max_angle = 180,
        .min_width_us = 500,
        .max_width_us = 2500,
        .freq = 50 ,
        .timer_number = LEDC_TIMER_0 ,
        .channels = {
            .servo_pin = {SERVO_PIN,    /*array in nested struct*/
            },
            .ch = {LEDC_CHANNEL_0,
            },
        },
        .channel_number =1,          /*total number of channels used*/      
        
    };

    iot_servo_init(LEDC_LOW_SPEED_MODE , &servo_cfg);
    
}

void servo_on()
{   
   float read_angle;
    float angle = 100.00f;
    iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0,angle);
    iot_servo_read_angle(LEDC_LOW_SPEED_MODE , 0, &read_angle);
    ESP_LOGI(TAG_SERVO, "servo current angle %f", read_angle);

}

