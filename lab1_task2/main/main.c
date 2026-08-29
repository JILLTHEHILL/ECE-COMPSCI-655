#include <driver/gpio.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#define BUTTON_PIN0 40
#define BUZZER_PIN0 38

#define DEBOUNCE_TIME 50000

static volatile int64_t last_interrupt_time = 0;

static void buzzer(void *arg)
{
  int64_t current_time = esp_timer_get_time();

  if (current_time - last_interrupt_time < DEBOUNCE_TIME)
  {
    return;
  }

  last_interrupt_time = current_time;
  gpio_set_level(BUZZER_PIN0, gpio_get_level(BUTTON_PIN0));
}

void app_main()
{
  gpio_config_t io_conf_1 = {
      .pin_bit_mask = (1ULL << BUTTON_PIN0),
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_ENABLE,
      .intr_type = GPIO_INTR_ANYEDGE,
  };
  gpio_config(&io_conf_1);

  gpio_config_t io_conf_2 = {
      .pin_bit_mask = (1ULL << BUZZER_PIN0),
      .mode = GPIO_MODE_OUTPUT,
  };
  gpio_config(&io_conf_2);

  gpio_install_isr_service(0);
  gpio_isr_handler_add(BUTTON_PIN0, buzzer, NULL);
}