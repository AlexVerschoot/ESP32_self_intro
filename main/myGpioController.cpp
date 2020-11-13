#include "myGpioController.h"
xQueueHandle MyGpioController::gpio_evt_queue;
int MyGpioController::debounceBool[24];
DisplaySSD1306_128x64_I2C* MyGpioController::display;

MyGpioController::MyGpioController(DisplaySSD1306_128x64_I2C * display)
{
    this->display = display;

    init();
}

void IRAM_ATTR MyGpioController::gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void MyGpioController::gpio_task_example(void* arg)
{
    char textChar[24];
    gpio_num_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            if(gpio_get_level(io_num)!=debounceBool[io_num]){
                printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
                debounceBool[io_num] = gpio_get_level(io_num);

                sprintf(textChar, "E : GPIO[%d] = %d", io_num, gpio_get_level(io_num));

                display->printFixed(16,8,textChar);
                //ssd1306_printFixed(0,  36, textChar, FONT_SIZE_NORMAL);
            }
        }
    }
}

MyGpioController::~MyGpioController()
{
}

void MyGpioController::init(){
   //using the normal gpio (gpio4) 
    gpio_config_t io_conf;
    //interrupt of rising edge
    io_conf.intr_type = gpio_int_type_t::GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4 here
    io_conf.pin_bit_mask = 4;
    //set as input mode    
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = gpio_pullup_t::GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    //change gpio intrrupt type for one pin
    gpio_set_intr_type(GPIO_NUM_4, GPIO_INTR_ANYEDGE);

    //get the initial gpio levels
    for (size_t i = 0; i < 24; i++)
    {
        debounceBool[i] = gpio_get_level((gpio_num_t)i);
    }
    

    //create a queue to handle gpio event from isr
    this->gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_NUM_4, gpio_isr_handler, (void*) 4);
}
