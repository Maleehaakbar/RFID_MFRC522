idf_component_register(SRCS "rc522.c" "spi.c"
                    INCLUDE_DIRS "." REQUIRES esp_driver_spi freertos log esp_driver_gpio)