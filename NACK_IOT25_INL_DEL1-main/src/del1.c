#include "app.h"
#include "config.h"
#include "gpio.h"
#include "pins.h"
#include "uart.h"
#include "millis.h"
#include "keypad.h"
#include <string.h>

#define PIN_LENGTH 4
#define INPUT_TIMEOUT_MS 5000
#define ACCESS_GRANTED_MS 3000
#define RED_BLINK_INTERVAL_MS 300
#define GREEN_FEEDBACK_MS 80

typedef enum
{
    STATE_IDLE,
    STATE_INPUT_AWAIT,
    STATE_ACCESS_GRANTED
} app_state_t;

static app_state_t current_state = STATE_IDLE;

static const char correct_pin[PIN_LENGTH + 1] = "1772";
static char entered_pin[PIN_LENGTH + 1];

static unsigned char pin_index = 0;

static millis_t state_start_time = 0;
static millis_t last_blink_time = 0;

static unsigned char red_led_is_on = 0;

static void enter_idle_state(void);
static void enter_input_await_state(void);
static void enter_access_granted_state(void);

static void handle_idle_state(void);
static void handle_input_await_state(void);
static void handle_access_granted_state(void);

static void green_feedback_blink(void);
static unsigned char pin_is_correct(void);

void app_init(void)
{
    gpio_pin_output(&RED_LED_DDR, RED_LED_PIN);
    gpio_pin_output(&GREEN_LED_DDR, GREEN_LED_PIN);

    gpio_pin_input_pullup(&GREEN_BUTTON_DDR, &GREEN_BUTTON_PORT, GREEN_BUTTON_PIN);

    keypad_init();

    millis_init();

    uart_init(UART_BAUDRATE);
    uart_write_string("Access system ready\n");

    enter_idle_state();
}

void app_run(void)
{
    switch (current_state)
    {
        case STATE_IDLE:
            handle_idle_state();
            break;

        case STATE_INPUT_AWAIT:
            handle_input_await_state();
            break;

        case STATE_ACCESS_GRANTED:
            handle_access_granted_state();
            break;
    }
}

static void enter_idle_state(void)
{
    current_state = STATE_IDLE;

    gpio_pin_high(&RED_LED_PORT, RED_LED_PIN);
    gpio_pin_low(&GREEN_LED_PORT, GREEN_LED_PIN);

    pin_index = 0;
    memset(entered_pin, 0, sizeof(entered_pin));

    state_start_time = millis_get();
}

static void enter_input_await_state(void)
{
    current_state = STATE_INPUT_AWAIT;

    gpio_pin_high(&RED_LED_PORT, RED_LED_PIN);
    gpio_pin_low(&GREEN_LED_PORT, GREEN_LED_PIN);

    red_led_is_on = 1;
    pin_index = 0;
    memset(entered_pin, 0, sizeof(entered_pin));

    state_start_time = millis_get();
    last_blink_time = millis_get();

    uart_write_string("Enter PIN\n");
}

static void enter_access_granted_state(void)
{
    current_state = STATE_ACCESS_GRANTED;

    gpio_pin_low(&RED_LED_PORT, RED_LED_PIN);
    gpio_pin_high(&GREEN_LED_PORT, GREEN_LED_PIN);

    state_start_time = millis_get();

    uart_write_string("Access granted\n");
}

static void handle_idle_state(void)
{
    gpio_pin_high(&RED_LED_PORT, RED_LED_PIN);
    gpio_pin_low(&GREEN_LED_PORT, GREEN_LED_PIN);

    if (gpio_pin_read(&GREEN_BUTTON_PINREG, GREEN_BUTTON_PIN) == 0)
    {
        millis_t start_time = millis_get();

        while (millis_get() - start_time < 30)
        {
        }

        if (gpio_pin_read(&GREEN_BUTTON_PINREG, GREEN_BUTTON_PIN) == 0)
        {
            enter_input_await_state();
        }
    }
}

static void handle_input_await_state(void)
{
    millis_t now = millis_get();

    if (now - last_blink_time >= RED_BLINK_INTERVAL_MS)
    {
        if (red_led_is_on)
        {
            gpio_pin_low(&RED_LED_PORT, RED_LED_PIN);
            red_led_is_on = 0;
        }
        else
        {
            gpio_pin_high(&RED_LED_PORT, RED_LED_PIN);
            red_led_is_on = 1;
        }

        last_blink_time = now;
    }

    if (now - state_start_time >= INPUT_TIMEOUT_MS)
    {
        uart_write_string("Timeout\n");
        enter_idle_state();
        return;
    }

    char key = keypad_scan();

    if (key != '\0')
    {
        green_feedback_blink();

        if (key >= '0' && key <= '9')
        {
            entered_pin[pin_index] = key;
            pin_index++;

            if (pin_index >= PIN_LENGTH)
            {
                entered_pin[PIN_LENGTH] = '\0';

                if (pin_is_correct())
                {
                    enter_access_granted_state();
                }
                else
                {
                    uart_write_string("Wrong PIN\n");
                    enter_idle_state();
                }
            }
        }
    }
}

static void handle_access_granted_state(void)
{
    gpio_pin_low(&RED_LED_PORT, RED_LED_PIN);
    gpio_pin_high(&GREEN_LED_PORT, GREEN_LED_PIN);

    if (millis_get() - state_start_time >= ACCESS_GRANTED_MS)
    {
        enter_idle_state();
    }
}

static void green_feedback_blink(void)
{
    gpio_pin_high(&GREEN_LED_PORT, GREEN_LED_PIN);

    millis_t start_time = millis_get();

    while (millis_get() - start_time < GREEN_FEEDBACK_MS)
    {
    }

    gpio_pin_low(&GREEN_LED_PORT, GREEN_LED_PIN);
}

static unsigned char pin_is_correct(void)
{
    if (strcmp(entered_pin, correct_pin) == 0)
    {
        return 1;
    }

    return 0;
}