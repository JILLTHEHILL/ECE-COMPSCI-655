#include <driver/gpio.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#define BUTTON_PIN0 40
#define BUZZER_PIN0 38
#define LED_PIN0 39

#define DEBOUNCE_TIME 50000

static bool led_state = false;

static volatile int64_t last_interrupt_time = 0;
static volatile bool button_pressed = false;

static int press_state = 0;

static esp_timer_handle_t led_timer;

static void led_blink(void *arg)
{
  led_state = !led_state;
  gpio_set_level(LED_PIN0, led_state);
}

static void buzzer(void *arg)
{
  int64_t current_time = esp_timer_get_time();

  // debounce
  if (current_time - last_interrupt_time < DEBOUNCE_TIME)
  {
    return;
  }

  last_interrupt_time = current_time;

  int button_state = gpio_get_level(BUTTON_PIN0);

  // buzzer follows button
  gpio_set_level(BUZZER_PIN0, button_state);

  // only count a PRESS, not a release
  if (button_state == 1)
  {
    button_pressed = true;
  }
}

void app_main()
{
  // BUTTON
  gpio_config_t button_conf = {
      .pin_bit_mask = (1ULL << BUTTON_PIN0),
      .mode = GPIO_MODE_INPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_ENABLE,
      .intr_type = GPIO_INTR_ANYEDGE,
  };

  gpio_config(&button_conf);

  // BUZZER
  gpio_config_t buzzer_conf = {
      .pin_bit_mask = (1ULL << BUZZER_PIN0),
      .mode = GPIO_MODE_OUTPUT,
  };

  gpio_config(&buzzer_conf);

  // LED
  gpio_config_t led_conf = {
      .pin_bit_mask = (1ULL << LED_PIN0),
      .mode = GPIO_MODE_OUTPUT,
  };

  gpio_config(&led_conf);

  // Create LED timer ONCE
  esp_timer_create_args_t timer_args = {
      .callback = &led_blink,
      .name = "led_timer",
  };

  esp_timer_create(&timer_args, &led_timer);

  // Enable button interrupt
  gpio_install_isr_service(0);
  gpio_isr_handler_add(BUTTON_PIN0, buzzer, NULL);

  while (1)
  {
    if (button_pressed)
    {
      button_pressed = false;

      press_state++;

      if (press_state > 3)
      {
        press_state = 0;
      }

      // FIRST PRESS: 1 Hz
      if (press_state == 1)
      {
        if (esp_timer_is_active(led_timer))
        {
          esp_timer_stop(led_timer);
        }

        esp_timer_start_periodic(led_timer, 1000000);
      }

      // SECOND PRESS: 2 Hz
      else if (press_state == 2)
      {
        esp_timer_stop(led_timer);

        esp_timer_start_periodic(led_timer, 500000);
      }

      // THIRD PRESS: 4 Hz
      else if (press_state == 3)
      {
        esp_timer_stop(led_timer);

        esp_timer_start_periodic(led_timer, 250000);
      }

      // FOURTH PRESS: OFF
      else
      {
        esp_timer_stop(led_timer);

        led_state = false;
        gpio_set_level(LED_PIN0, 0);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}