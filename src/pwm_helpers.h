#ifndef PWM_HELPERS_H
#define PWM_HELPERS_H
#include <stdint.h>
#include <hardware/pwm.h>
#include <hardware/gpio.h>
#include <hardware/structs/io_bank0.h>

inline static void pwm_init_pin(uint8_t pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Set the wrap value of the entire slice.
    uint slice_number = pwm_gpio_to_slice_num(pin);
    pwm_set_wrap(slice_number, UINT16_MAX);

    // Begin PWM on the pin.
    pwm_set_enabled(slice_number, true);
}

inline static void pwm_write(uint8_t output_pin, uint8_t value) {
    uint16_t scaled_value = (uint16_t) (value * (float) UINT16_MAX / UINT8_MAX);
    pwm_set_gpio_level(output_pin, scaled_value);
}

#endif