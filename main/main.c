#include <stdio.h>
#include "rc522.h"
#include "spi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

  configRF conf;

   

void app_main(void)
{ 
    spi_init();
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
      RFID_transceive(&conf);
  
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    
    
}