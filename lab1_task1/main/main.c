#include <driver/gpio.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#define LED_PIN_1 38
#define LED_PIN_2 39

static bool led_state_1 = false;
static bool led_state_2 = false;

static void led_blink_1(void *arg)
{
  led_state_1 = !led_state_1;
  gpio_set_level(LED_PIN_1, led_state_1);
}

static void led_blink_2(void *arg)
{
  led_state_2 = !led_state_2;
  gpio_set_level(LED_PIN_2, led_state_2);
}

void app_main()
{

  // Configure the LED pin
  gpio_config_t io_conf_1 = {
      .pin_bit_mask = (1ULL << LED_PIN_1),
      .mode = GPIO_MODE_OUTPUT,
  };
  gpio_config(&io_conf_1);

  // Configure the second LED pin
  gpio_config_t io_conf_2 = {
      .pin_bit_mask = (1ULL << LED_PIN_2),
      .mode = GPIO_MODE_OUTPUT,
  };
  gpio_config(&io_conf_2);

  // Create a timer
  esp_timer_create_args_t timer_args = {
      .callback = &led_blink_1,
      .name = "timer_1",
  };

  esp_timer_handle_t timer_1;
  esp_timer_create(&timer_args, &timer_1);
  esp_timer_start_periodic(timer_1, 500000);

  esp_timer_create_args_t timer_args_2 = {
      .callback = &led_blink_2,
      .name = "timer_2",
  };

  esp_timer_handle_t timer_2;
  esp_timer_create(&timer_args_2, &timer_2);
  esp_timer_start_periodic(timer_2, 1000000);
}
