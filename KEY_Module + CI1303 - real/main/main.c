#include <stdio.h>
#include "bsp_power.h"
#include "bsp_i2c.h"
#include "bsp_XL9555.h"
#include "bsp_key.h"
#include "bsp_usart.h"
// #include "nvs_flash.h"
#include "ci1303moduletask.h"
#include "keymoduletask.h"
#include "esp_spiffs.h"

//  storage,  data,     spiffs,      0xc00000,   0x400000, ,

void app_main(void)
{
    // esp_err_t ret;
    // // esp_partition_t* partition_ptr = NULL;
    
    // ret= nvs_flash_init();  /* 初始化NVS */

    // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    // {
    //     ESP_ERROR_CHECK(nvs_flash_erase());
    //     ret = nvs_flash_init();
    // }

    // partition_ptr = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);

    // esp_partition_erase_range(partition_ptr, 0, 0x7D000);

    // esp_partition_write_raw();

    // LED_Init();
    Power_Init();
    I2C_Init();
    Xl9555_init();
    Key_Init();
    Usart_Init(115200);
                      
    vTaskDelay(500/portTICK_PERIOD_MS);

    printf("freertos Start\r\n");

    CI1303_USART_Rec_Start();
    CI1303_USART_Action_Start();

    KEYScan_Start();
    KEYScan_Action_Start();
}
