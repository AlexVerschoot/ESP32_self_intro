#ifndef MYGPIOCONTROLLER_H
#define MYGPIOCONTROLLER_H

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

class MyGpioController
{
private:
    void init();

    static DisplaySSD1306_128x64_I2C* display;


    static xQueueHandle gpio_evt_queue;
    static int debounceBool[24];

    static void IRAM_ATTR gpio_isr_handler(void* arg);

    static void gpio_task_example(void* arg);



    /* data */
public:
    //MyGpioController(const NanoDisplayOps<class O, class I>* display);
    MyGpioController(DisplaySSD1306_128x64_I2C * display);

    ~MyGpioController();

};



#endif