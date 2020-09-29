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
#include "ssd1306.h"
#include "hal/gpio_types.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#define CHIP_NAME "ESP32"
#endif

#ifdef CONFIG_IDF_TARGET_ESP32S2BETA
#define CHIP_NAME "ESP32-S2 Beta"
#endif


static xQueueHandle gpio_evt_queue = NULL;
static int debounceBool[24];

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void* arg)
{
    char textChar[24];
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            if(gpio_get_level(io_num)!=debounceBool[io_num]){
                printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
                debounceBool[io_num] = gpio_get_level(io_num);

                sprintf(textChar, "E : GPIO[%d] = %d", io_num, gpio_get_level(io_num));
                ssd1306_printFixed(0,  36, textChar, STYLE_NORMAL);
            }
        }
    }
}



void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_128x64_i2c_init();
    ssd1306_clearScreen();

    char textChar[36];
    esp_err_t errorVar;

    
//using the normal gpio (gpio4) 
    gpio_config_t io_conf;
    //interrupt of rising edge
    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4 here
    io_conf.pin_bit_mask = 4;
    //set as input mode    
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //change gpio intrrupt type for one pin
    gpio_set_intr_type(4, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(4, gpio_isr_handler, (void*) 4);


//using the RTC_gpio (rtc_gpio10)

    while (true)
    {
                printf("This is %s chip with %d CPU cores, WiFi%s%s, ",
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
    }
}
