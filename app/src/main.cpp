#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#define COMM_LED_NODE   DT_ALIAS(comm_led)
#define STATUS_LED_NODE DT_ALIAS(status_led)
#define POWER_LED_NODE  DT_ALIAS(power_led)
#define MY_SW  DT_ALIAS(my_sw)


static const struct gpio_dt_spec led_0 = GPIO_DT_SPEC_GET(COMM_LED_NODE, gpios);
static const struct gpio_dt_spec led_1 = GPIO_DT_SPEC_GET(STATUS_LED_NODE, gpios);
static const struct gpio_dt_spec led_2 = GPIO_DT_SPEC_GET(POWER_LED_NODE, gpios);

static const struct gpio_dt_spec sw = GPIO_DT_SPEC_GET(MY_SW, gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    bool led_state = true;
    bool prev_sw_state = 0;

    if (!gpio_is_ready_dt(&led_0)) return 0;
    if (!gpio_is_ready_dt(&led_1)) return 0;
    if (!gpio_is_ready_dt(&led_2)) return 0;
    if (!gpio_is_ready_dt(&sw)) return 0;

    if (gpio_pin_configure_dt(&led_0, GPIO_OUTPUT_ACTIVE) < 0) return 0;
    if (gpio_pin_configure_dt(&led_1, GPIO_OUTPUT_ACTIVE) < 0) return 0;
    if (gpio_pin_configure_dt(&led_2, GPIO_OUTPUT_ACTIVE) < 0) return 0;
    if (gpio_pin_configure_dt(&sw, GPIO_INPUT) < 0) return 0;

    while (1) {
        if (gpio_pin_toggle_dt(&led_0) < 0) return 0;
        if (gpio_pin_toggle_dt(&led_1) < 0) return 0;

        int sw_state = gpio_pin_get_dt(&sw);
		if (sw_state < 0) return 0;

		if (sw_state == 1 && prev_sw_state == 0) {
			gpio_pin_toggle_dt(&led_2);
			LOG_INF("Button Pressed");
		}

        led_state = !led_state;
        LOG_INF("LED state: %s", led_state ? "ON" : "OFF");
        k_msleep(CONFIG_BLINK_SLEEP_TIME_MS);
    }

    return 0;
}