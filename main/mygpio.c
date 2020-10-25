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
#include "ssd1306.h"

static int debounceBool[24];
static xQueueHandle gpio_evt_queue = NULL;






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

                
                ssd1306_printFixed(0,  36, textChar, FONT_SIZE_NORMAL);
            }
        }
    }
}

void mygpio_init(){

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
}