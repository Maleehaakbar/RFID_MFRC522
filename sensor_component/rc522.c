#include "rc522.h"
#include "spi.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const char * TAG_RFID ="RFID";
 uint8_t rcv_buff[5];          /*buffer to receive uuid bytes and BCC*/
 uint8_t txlastbits = false;
#define START_SEND 1
#define txWaitRF 1

#define CRCIRq_mask  0x20

void RFID_transceive(configRF *config_RF);

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
{   
    uint8_t fifo_level;

    /*send transcieve command*/
    spi_write(WRITE_REG(CommandReg), (config_RF->rcv << BIT_NO_5| config_RF->command << BIT_NO_0));  
    
    if(txlastbits)
    {   
        /*start send and set bit length to 7*/
        spi_write(WRITE_REG(BitFramingReg),START_SEND << BIT_NO_7 | 7 << BIT_NO_0);   
    }

    else 
    {   
        /*start send */
        spi_write(WRITE_REG(BitFramingReg),START_SEND << BIT_NO_7);
    }
    
    vTaskDelay(pdMS_TO_TICKS(500));

    /*stop transmision*/
    spi_write(WRITE_REG(BitFramingReg),0 << BIT_NO_7 );         
    spi_write(WRITE_REG(CommandReg), (config_RF->rcv << BIT_NO_5| IDLE << BIT_NO_0));

    /*read fifo level*/
    fifo_level= spi_read(READ_REG(FIFOLevelReg));
    ESP_LOGI(TAG_RFID, "received fifo level %x", fifo_level);

}

uint8_t RFID_tag_detect(configRF *config_RF)
{
    uint8_t buff[2];
    uint16_t resp;
    txlastbits = true;
    vTaskDelay(pdMS_TO_TICKS(5));

    spi_write(WRITE_REG(FIFODataReg), REQA);    
    RFID_transceive(config_RF);
    
    for(uint8_t i=0; i<2; i++)
    {   
        /*read ATQA*/
        buff[i] = spi_read(READ_REG(FIFODataReg));     
        ESP_LOGI(TAG_RFID, "received byte from tag %x", buff[i]); 
    }

    resp = buff[0] <<8 | buff[1];

    /*Flush the buffer*/
    spi_write(WRITE_REG(FIFOLevelReg), 1<< BIT_NO_7);  

    if(resp == ATQA_COMMAND)
    {
        ESP_LOGI(TAG_RFID, "TAG detected in RFID field");
        return 1;
    }

    else 
    {
        ESP_LOGE(TAG_RFID, "No tag detected");
        return 0;
    }
}

void RFID_anticollision(configRF *config_RF)
{  
    uint8_t uuid_xor;
    txlastbits = false;
    spi_write(WRITE_REG(FIFODataReg), SEL_CL1);
    spi_write(WRITE_REG(FIFODataReg), ANTI_COLL);
    RFID_transceive(config_RF);

    for(uint8_t i=0; i<5; i++)
    {
        rcv_buff[i] = spi_read(READ_REG(FIFODataReg));    
        ESP_LOGI(TAG_RFID, "received uuid bytes %x", rcv_buff[i]); 
    }
    uuid_xor = rcv_buff[0] ^ rcv_buff[1] ^ rcv_buff[2] ^ rcv_buff[3];

    if (uuid_xor != rcv_buff[4])
    {
        ESP_LOGI(TAG_RFID, "invalid response from tag");
    }
    
    else 
    {
        ESP_LOGI(TAG_RFID, "received valid uuid"); 
    }

    spi_write(WRITE_REG(FIFOLevelReg), 1<< BIT_NO_7);
}

void RFID_send_SAK(configRF *config_RF)
{   uint8_t CRC_status, CRC_H, CRC_L, SAK_byte;
    txlastbits = false;
    spi_write(WRITE_REG(FIFODataReg), SEL_CL1);
    spi_write(WRITE_REG(FIFODataReg), SELECT_COMMAND);
    
    for (uint8_t i=0; i<5; i++)
    {
        spi_write(WRITE_REG(FIFODataReg), rcv_buff[i]);
    }
    
    /*calcuate CRC over FIFO content*/
    spi_write(WRITE_REG(CommandReg), CALC_CRC << BIT_NO_0); 

    do 
    {
        CRC_status = spi_read(READ_REG(Status1Reg));

    }while((CRC_status & CRCIRq_mask)!= CRCIRq_mask);

    CRC_L = spi_read(READ_REG(CRCResultRegL));
    CRC_H = spi_read(READ_REG(CRCResultRegH));
    
    spi_write(WRITE_REG(CommandReg), (config_RF->rcv << BIT_NO_5| IDLE << BIT_NO_0));
    spi_write(WRITE_REG(FIFOLevelReg), 1<< BIT_NO_7); 
    
    spi_write(WRITE_REG(FIFODataReg), SEL_CL1);
    spi_write(WRITE_REG(FIFODataReg), SELECT_COMMAND);
    
    for (uint8_t i=0; i<5; i++)
    {
        spi_write(WRITE_REG(FIFODataReg), rcv_buff[i]);
        ESP_LOGI(TAG_RFID, "send data from master %x", rcv_buff[i]);
    }

    spi_write(WRITE_REG(FIFODataReg), CRC_L);
    spi_write(WRITE_REG(FIFODataReg), CRC_H);
    RFID_transceive(config_RF);
    SAK_byte = spi_read(READ_REG(FIFODataReg));
    ESP_LOGI(TAG_RFID, "SAK byte %x", SAK_byte );
    if ((SAK_byte & 0x4) ==0X4)
    {
        ESP_LOGW(TAG_RFID, "uuid incomplete");
    }

    else
    {
        ESP_LOGI(TAG_RFID, "uuid complete"); 
    }
}




