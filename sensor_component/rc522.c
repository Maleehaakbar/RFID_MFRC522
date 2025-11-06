#include "rc522.h"
#include "spi.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const char * TAG_RFID ="RFID";
#define START_SEND 1
#define txWaitRF 1
#define REQA      0x26


void RFID_config(configRF *config_RF)
{   

    spi_write(WRITE_REG(CommandReg), SOFT_RESET << BIT_NO_0);
    vTaskDelay(pdMS_TO_TICKS(500));
    spi_write(WRITE_REG(TxModeReg), (config_RF->TxCRCEnable << BIT_NO_7 | config_RF->TxSpeed << BIT_NO_4));
    spi_write(WRITE_REG(RxModeReg), (config_RF->RxCRCEnable << BIT_NO_7 | config_RF->RxSpeed << BIT_NO_4 | config_RF->Rx_multiple << BIT_NO_2));
    spi_write(WRITE_REG(TxControlReg), (config_RF->Tx2CW << BIT_NO_3 | config_RF->Tx2RFEnable << BIT_NO_1 | config_RF->Tx1RFEnable << BIT_NO_0));
    spi_write(WRITE_REG(TxASKReg),(config_RF->Force100ASK << BIT_NO_6 ));
    spi_write(WRITE_REG(TxSelReg), (config_RF->driver_sel << BIT_NO_4));
    spi_write(WRITE_REG(DemodReg), (config_RF->TauRcv << BIT_NO_2));
    
}

void RFID_transceive(configRF *config_RF)
{   uint8_t buff[2];
    uint8_t fifo_level;
    
    vTaskDelay(pdMS_TO_TICKS(5));
    spi_write(WRITE_REG(FIFODataReg), REQA);           /*send request(REQA) command*/
    spi_write(WRITE_REG(BitFramingReg),7 << BIT_NO_0); 
    spi_write(WRITE_REG(CommandReg), (config_RF->rcv << BIT_NO_5| config_RF->command << BIT_NO_0));  //send transcieve command
    spi_write(WRITE_REG(BitFramingReg),START_SEND << BIT_NO_7 | 7 << BIT_NO_0);   //set start_send bit
    vTaskDelay(pdMS_TO_TICKS(500));
    spi_write(WRITE_REG(BitFramingReg),0 << BIT_NO_7 );          //stop send
 
    vTaskDelay(pdMS_TO_TICKS(500));
   
    for(uint8_t i=0; i< 2; i++)
    {
        buff[i] = spi_read(READ_REG(FIFODataReg));     /*read ATQA*/
        ESP_LOGI(TAG_RFID, "received byte from tag %x", buff[i]); 
    }

    fifo_level= spi_read(READ_REG(FIFOLevelReg));
 
    spi_write(WRITE_REG(CommandReg), (config_RF->rcv << BIT_NO_5| IDLE << BIT_NO_0));
    ESP_LOGI(TAG_RFID, "received fifo level %x", fifo_level<<1);
}



//remove flush buffer command , change sequece of bit framing reg
