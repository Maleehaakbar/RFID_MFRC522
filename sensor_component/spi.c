#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "sdkconfig.h"
#include "esp_log.h"


#define RFID_HOST      SPI2_HOST
// Use default pins, same as the flash chip.
#define PIN_NUM_MISO     18
#define PIN_NUM_MOSI     16
#define PIN_NUM_CLK      17
#define PIN_NUM_CS       15

static const char* TAG = "SPI_INTERFACE";
spi_device_handle_t spi_handle;

void spi_init(void)
{
  esp_err_t ret;
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };

    ret = spi_bus_initialize(RFID_HOST, &buscfg, SPI_DMA_DISABLED);
    if(ret!= ESP_OK)
    {
        ESP_LOGE(TAG, "SPI bus init failed");
    }

    spi_device_interface_config_t slave_dev = {
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size =5,
        .address_bits = 8,      //length of address bits
        .input_delay_ns = 10,
        .clock_source = SPI_CLK_SRC_DEFAULT,
        .clock_speed_hz = 4 * 1000 * 1000,

    };

    ret = spi_bus_add_device(RFID_HOST, &slave_dev, &spi_handle);
    if(ret!= ESP_OK)
    {
        ESP_LOGE(TAG, "failed t0 add SPI device");
    }
}

void spi_write(uint8_t addr, uint8_t tx_data)
{   
    esp_err_t ret;
    static spi_transaction_t write_trans;
    
    memset(&write_trans, 0 , sizeof(spi_transaction_t));
    write_trans.addr = (uint64_t)addr;
    write_trans.length = 8;           //length of data bits for write phase
    write_trans.flags = SPI_TRANS_USE_TXDATA;
    write_trans.tx_data[0] = tx_data;
    ret = spi_device_polling_transmit(spi_handle,&write_trans);
    if(ret!=ESP_OK)
    {
        ESP_LOGE(TAG, "SPI write unsuccessful");
    }
    
    /*else 
    {   
        ESP_LOGI(TAG, "send data from master %x",write_trans.tx_data[0] );
    }*/


}


uint8_t spi_read( uint8_t addr)
{   
    uint8_t buff;
    esp_err_t ret;
    static spi_transaction_t read_trans;
    memset(&read_trans, 0 , sizeof(spi_transaction_t));
    read_trans.addr = addr;
    read_trans.length = 8;
    read_trans.rxlength = 8;
    read_trans.flags = SPI_TRANS_USE_RXDATA;
  
    ret = spi_device_polling_transmit(spi_handle,&read_trans);

    buff = read_trans.rx_data[0];
    if(ret!=ESP_OK)
    {
        ESP_LOGE(TAG, "SPI read unsuccessful");
    }
    
    /*else 
    {
        ESP_LOGI(TAG, "SPI read successful");
    }*/

    return buff;
    
}