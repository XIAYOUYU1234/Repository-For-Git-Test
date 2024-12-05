#include "bsp_i2c.h"

void I2C_Init(void)
{
    i2c_config_t i2c_config_struct = {0};
    esp_err_t ret = ESP_OK;

    i2c_config_struct.master.clk_speed = 400000;
    i2c_config_struct.mode = I2C_MODE_MASTER;
    i2c_config_struct.scl_io_num = GPIO_NUM_18;
    i2c_config_struct.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config_struct.sda_io_num = GPIO_NUM_17;
    i2c_config_struct.sda_pullup_en = GPIO_PULLUP_ENABLE;

    i2c_param_config(I2C_NUM_1, &i2c_config_struct);
    
    ret = i2c_driver_install(I2C_NUM_1, i2c_config_struct.mode, 0, 0, 0);

    if(ret != ESP_OK)
    {
        printf("%s , ret: %d", __func__, ret);
        while(1);
    }
}