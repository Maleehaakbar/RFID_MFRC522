#include "esp_lcd_panel_ssd1306.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"
#include "oled.h"
#include "rc522.h"


#define I2C_MASTER_SDA_IO  5
#define I2C_MASTER_SCL_IO  6
#define I2C_SLAVE_NUM    I2C_NUM_0
#define TEST_I2C_PORT    I2C_SLAVE_NUM
#define DEVICE_ADDR        0x3C

#define LCD_CMD_BITS           8
#define LCD_PARAM_BITS         8

#define LCD_PIXEL_CLOCK_HZ    (400 * 1000)
#define LCD_HEIGHT             32
#define LCD_WIDTH              128

const char* TAG_OLED = "OLED_INTERFACE";
lv_disp_t *disp = NULL;
lv_obj_t *label = NULL;

i2c_master_bus_config_t i2c_mst_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = TEST_I2C_PORT,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
};


i2c_master_bus_handle_t bus_handle;

esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = DEVICE_ADDR,
        .scl_speed_hz = LCD_PIXEL_CLOCK_HZ,
        .control_phase_bytes = 1,               // According to SSD1306 datasheet
        .lcd_cmd_bits = LCD_CMD_BITS,   // According to SSD1306 datasheet
        .lcd_param_bits = LCD_PARAM_BITS, // According to SSD1306 datasheet
        .dc_bit_offset = 6,                     // According to SSD1306 datasheet

};

esp_lcd_panel_io_handle_t io_handle = NULL;
esp_lcd_panel_handle_t panel_handle = NULL;

esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = -1,
};

esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = LCD_HEIGHT,
    };


void oled_init(void)
{   esp_err_t ret;

    ret = i2c_new_master_bus(&i2c_mst_config, &bus_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG_OLED,"Failed to initialize I2C bus");
    }
    

    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(bus_handle, &io_config, &io_handle));
    ESP_LOGI(TAG_OLED, "Install SSD1306 panel driver");
   
  
    panel_config.vendor_config = &ssd1306_config;
    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    
    
}

void lvgl_init()
{
    ESP_LOGI(TAG_OLED, "Initialize LVGL");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size =  LCD_HEIGHT * LCD_WIDTH,
        .double_buffer = true,
        .hres = LCD_WIDTH,
        .vres = LCD_HEIGHT,
        .monochrome = true,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        }
    };

    disp = lvgl_port_add_disp(&disp_cfg);
   
}

void lvgl_write(lv_disp_t *disp)
{   
     /* Rotation of the screen */
    lv_disp_set_rotation(disp, LV_DISP_ROT_NONE);  
    if (lvgl_port_lock(0)) {
        lv_obj_t *scr = lv_disp_get_scr_act(disp);
        label = lv_label_create(scr);
        lv_label_set_text(label, "Hello Espressif\n");
      
        // Release the mutex
        lvgl_port_unlock();
    }
}



