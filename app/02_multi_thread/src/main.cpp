#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>


#define APP_LED_NODE DT_ALIAS(app_led)
#define STATUS_LED_NODE DT_ALIAS(status_led)

#define LED0_THREAD_STACK_SIZE 512
#define LED1_THREAD_STACK_SIZE 512

K_THREAD_STACK_DEFINE(led0_thread_stack, LED0_THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(led1_thread_stack, LED1_THREAD_STACK_SIZE);

static struct k_thread led0_thread_data;
static struct k_thread led1_thread_data;

static const int32_t led0_sleep_time_ms = 500;
static const int32_t led1_sleep_time_ms = 1000;

static const struct gpio_dt_spec led_0 = GPIO_DT_SPEC_GET(APP_LED_NODE, gpios);
static const struct gpio_dt_spec led_1 = GPIO_DT_SPEC_GET(STATUS_LED_NODE, gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

K_SEM_DEFINE(led_sem, 0, 1);

void led0_thread(void *, void *, void *){

    bool led_state = true;
    if (!gpio_is_ready_dt(&led_0)) return;
    if (gpio_pin_configure_dt(&led_0, GPIO_OUTPUT_ACTIVE) < 0) return;

    while (1) {
        if (gpio_pin_toggle_dt(&led_0) < 0) return;

        led_state = !led_state;
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        k_sem_give(&led_sem);         // signal LED1
        k_msleep(led0_sleep_time_ms);
    }
}

void led1_thread(void *, void *, void *){

    bool led_state = true;
    if (!gpio_is_ready_dt(&led_1)) return;
    if (gpio_pin_configure_dt(&led_1, GPIO_OUTPUT_ACTIVE) < 0) return;

    while (1) {
        k_sem_take(&led_sem, K_FOREVER);
        
        if (gpio_pin_toggle_dt(&led_1) < 0) return;

        led_state = !led_state;
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        k_msleep(led1_sleep_time_ms);
    }

}

int main(){
    k_tid_t led0_tid = k_thread_create(&led0_thread_data, 
                                    led0_thread_stack, 
                                    LED0_THREAD_STACK_SIZE,
                                    led0_thread, 
                                    NULL, 
                                    NULL, 
                                    NULL,
                                    2, 
                                    0, 
                                    K_NO_WAIT);

    k_tid_t led1_tid = k_thread_create(&led1_thread_data, 
                                led1_thread_stack, 
                                LED1_THREAD_STACK_SIZE,    
                                led1_thread, 
                                NULL, 
                                NULL, 
                                NULL,
                                1, 
                                0, 
                                K_NO_WAIT);

    while(1){
        k_msleep(1000);
    }

    return 0;
}