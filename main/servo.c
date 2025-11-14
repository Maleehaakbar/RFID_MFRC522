#include "iot_servo.h"
#include "esp_log.h"
#include "servo.h"
#include "freertos/FreeRTOS.h"

#define SERVO_PIN  4

const char* TAG_SERVO = "servo_app";
void servo_on();
void servo_init()
{   
    servo_config_t servo_cfg = {
        .max_angle = 180,
        .min_width_us = 500,   /*0.5ms = 0 (angle) = 2.5% duty cycle*/
        .max_width_us = 2500,  /*2.5ms =180(angle) =2.4% PWM duty cycle*/
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
    for (uint8_t i=0; i<30; i++)
    {
        iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0,i);
    }
  

}

