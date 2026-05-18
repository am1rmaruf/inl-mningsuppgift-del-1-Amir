#include "keypad.h"
#include "gpio.h"
#include "pins.h"
#include "millis.h"
#include <stdint.h>

static const char keys[4][4] =
{
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void keypad_init(void)
{
    gpio_pin_output(&KEYPAD_ROW1_DDR, KEYPAD_ROW1_PIN);
    gpio_pin_output(&KEYPAD_ROW2_DDR, KEYPAD_ROW2_PIN);
    gpio_pin_output(&KEYPAD_ROW3_DDR, KEYPAD_ROW3_PIN);
    gpio_pin_output(&KEYPAD_ROW4_DDR, KEYPAD_ROW4_PIN);

    gpio_pin_high(&KEYPAD_ROW1_PORT, KEYPAD_ROW1_PIN);
    gpio_pin_high(&KEYPAD_ROW2_PORT, KEYPAD_ROW2_PIN);
    gpio_pin_high(&KEYPAD_ROW3_PORT, KEYPAD_ROW3_PIN);
    gpio_pin_high(&KEYPAD_ROW4_PORT, KEYPAD_ROW4_PIN);

    gpio_pin_input_pullup(&KEYPAD_COL1_DDR, &KEYPAD_COL1_PORT, KEYPAD_COL1_PIN);
    gpio_pin_input_pullup(&KEYPAD_COL2_DDR, &KEYPAD_COL2_PORT, KEYPAD_COL2_PIN);
    gpio_pin_input_pullup(&KEYPAD_COL3_DDR, &KEYPAD_COL3_PORT, KEYPAD_COL3_PIN);
    gpio_pin_input_pullup(&KEYPAD_COL4_DDR, &KEYPAD_COL4_PORT, KEYPAD_COL4_PIN);
}

char keypad_scan(void)
{
    volatile uint8_t *row_ports[4] =
    {
        &KEYPAD_ROW1_PORT,
        &KEYPAD_ROW2_PORT,
        &KEYPAD_ROW3_PORT,
        &KEYPAD_ROW4_PORT
    };

    uint8_t row_pins[4] =
    {
        KEYPAD_ROW1_PIN,
        KEYPAD_ROW2_PIN,
        KEYPAD_ROW3_PIN,
        KEYPAD_ROW4_PIN
    };

    volatile uint8_t *col_pinregs[4] =
    {
        &KEYPAD_COL1_PINREG,
        &KEYPAD_COL2_PINREG,
        &KEYPAD_COL3_PINREG,
        &KEYPAD_COL4_PINREG
    };

    uint8_t col_pins[4] =
    {
        KEYPAD_COL1_PIN,
        KEYPAD_COL2_PIN,
        KEYPAD_COL3_PIN,
        KEYPAD_COL4_PIN
    };

    for (uint8_t row = 0; row < 4; row++)
    {
        gpio_pin_high(&KEYPAD_ROW1_PORT, KEYPAD_ROW1_PIN);
        gpio_pin_high(&KEYPAD_ROW2_PORT, KEYPAD_ROW2_PIN);
        gpio_pin_high(&KEYPAD_ROW3_PORT, KEYPAD_ROW3_PIN);
        gpio_pin_high(&KEYPAD_ROW4_PORT, KEYPAD_ROW4_PIN);

        gpio_pin_low(row_ports[row], row_pins[row]);

        for (uint8_t col = 0; col < 4; col++)
        {
            if (gpio_pin_read(col_pinregs[col], col_pins[col]) == 0)
            {
                millis_t start_time = millis_get();

                while (millis_get() - start_time < 30)
                {
                }

                if (gpio_pin_read(col_pinregs[col], col_pins[col]) == 0)
                {
                    while (gpio_pin_read(col_pinregs[col], col_pins[col]) == 0)
                    {
                    }

                    gpio_pin_high(row_ports[row], row_pins[row]);

                    return keys[row][col];
                }
            }
        }

        gpio_pin_high(row_ports[row], row_pins[row]);
    }

    return '\0';
}