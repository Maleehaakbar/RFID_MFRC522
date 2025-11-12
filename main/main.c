#include <stdio.h>
#include "rc522.h"
#include "spi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "servo.h"
#include "iot_servo.h"

configRF conf;
uint8_t tag_detect;
static const char* TAG_MAIN = "RFID_APP";
   

void app_main(void)
{ 
    spi_init();
    servo_init();
    conf.rcv = rcv_on;
    conf.command = TRANSCEIVE;
    conf.driver_sel = 1;
    conf.Force100ASK = 1;
    conf.Rx_multiple = 0;
    conf.Tx2CW = 0;
    conf.Tx2RFEnable = ENABLE;
    conf.Tx1RFEnable = ENABLE;
    conf.RxSpeed = TX_RATE_106_KBD;
    conf.TxSpeed = TX_RATE_106_KBD;
    conf.RxCRCEnable = DISABLE;
    conf.TauRcv = 1;
    
    while(1)
    {     
      RFID_config(&conf);
      tag_detect = RFID_tag_detect(&conf);
      if(tag_detect)
      { 
        iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0,180);
        RFID_anticollision(&conf);
        RFID_send_SAK(&conf);
        iot_servo_write_angle(LEDC_LOW_SPEED_MODE, 0,0);
      }
      else 
      {
        ESP_LOGW(TAG_MAIN, "Wait for TAG");
      }
      vTaskDelay(pdMS_TO_TICKS(500));
    }
    
    
}