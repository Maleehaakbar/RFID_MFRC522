#include <stdio.h>
#include "rc522.h"
#include "spi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "servo.h"
#include "iot_servo.h"
#include "oled.h"
#include "lvgl.h"


uint8_t tag_detect;
static const char* TAG_MAIN = "RFID_APP";
uint32_t display_uuid;

configRF conf = {
    .rcv = rcv_on,
    .command = TRANSCEIVE,
    .driver_sel = 1,
    .Force100ASK = 1,
    .Rx_multiple = 0,
    .Tx2CW = 0,
    .Tx2RFEnable = ENABLE,
    .Tx1RFEnable = ENABLE,
    .RxSpeed = TX_RATE_106_KBD,
    .TxSpeed = TX_RATE_106_KBD,
    .RxCRCEnable = DISABLE,
    .TauRcv = 1,
};

void app_main(void)
{ 
    spi_init();
    servo_init();
    oled_init();
    lvgl_init();
    lvgl_write(disp);
    
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
        display_uuid = uuid_bytes();
        lv_label_set_text_fmt(label,"TAG UUID : %lx", display_uuid);
      }
      else 
      {
        ESP_LOGW(TAG_MAIN, "Wait for TAG");
        lv_label_set_text(label, "No tag present\n");
      
      }
   
      vTaskDelay(pdMS_TO_TICKS(500));
    }
    
    
}