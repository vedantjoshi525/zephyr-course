#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define APP_LED_NODE DT_ALIAS(app_led)
#define STATUS_LED_NODE DT_ALIAS(status_led)
#define POWER_LED_NODE  DT_ALIAS(power_led)

static const struct gpio_dt_spec led_0 = GPIO_DT_SPEC_GET(APP_LED_NODE, gpios);
static const struct gpio_dt_spec led_1 = GPIO_DT_SPEC_GET(STATUS_LED_NODE, gpios);
static const struct gpio_dt_spec led_2 = GPIO_DT_SPEC_GET(POWER_LED_NODE, gpios);

int main(void)
{
    bool led_state = true;

    if (!gpio_is_ready_dt(&led_0)) return 0;
    if (!gpio_is_ready_dt(&led_1)) return 0;
    if (!gpio_is_ready_dt(&led_2)) return 0;

    if (gpio_pin_configure_dt(&led_0, GPIO_OUTPUT_ACTIVE) < 0) return 0;
    if (gpio_pin_configure_dt(&led_1, GPIO_OUTPUT_ACTIVE) < 0) return 0;
    if (gpio_pin_configure_dt(&led_2, GPIO_OUTPUT_ACTIVE) < 0) return 0;

    while (1) {
        if (gpio_pin_toggle_dt(&led_0) < 0) return 0;
        if (gpio_pin_toggle_dt(&led_1) < 0) return 0;
        if (gpio_pin_toggle_dt(&led_2) < 0) return 0;


        led_state = !led_state;
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        k_msleep(CONFIG_BLINK_SLEEP_TIME_MS);
    }

    return 0;
}