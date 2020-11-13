/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "hal/gpio_types.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "lcdgfx.h"
#include "lcdgfx_gui.h"
#include "myGpioController.h"
//#include "mygpio.c"

#ifdef CONFIG_IDF_TARGET_ESP32
#define CHIP_NAME "ESP32"
#endif

#ifdef CONFIG_IDF_TARGET_ESP32S2BETA
#define CHIP_NAME "ESP32-S2 Beta"
#endif








extern "C" void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);



    char textChar[36];
    esp_err_t errorVar;

    //mygpio_init();

    DisplaySSD1306_128x64_I2C display(-1);
    //DisplaySSD1306_128x64_SPI display(22,{-1, 5, 21, 0,-1,-1}); // Use this line for ESP32 (VSPI)  (gpio22=RST, gpio5=CE for VSPI, gpio21=D/C)
    display.begin();
    display.setFixedFont(ssd1306xled_font8x16);
    display.clear();
    display.printFixed(16,8,"hello world");

    MyGpioController mygpio(&display);

//using the RTC_gpio (rtc_gpio10)

    //suspend ourselves, the rest is up to other functions
    vTaskSuspend( NULL );

    while (true)
    {

        /*printf("This is %s chip with %d CPU cores, WiFi%s%s, ",
                CHIP_NAME,
                chip_info.cores,
                (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
                (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

        printf("silicon revision %d, ", chip_info.revision);

        printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
                (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

        for (int i = 1000; i >= 0; i--) {
            sprintf(textChar, "Restarting in %d seconds...", i);
            //printf(textChar);
            //ssd1306_clearScreen();
            //ssd1306_printFixed(0,  8, textChar, STYLE_NORMAL);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            fflush(stdout);
        }
        printf("Restarting now.\n");
        */
    }
}
