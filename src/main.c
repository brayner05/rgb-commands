#include <errno.h>
#include <hardware/gpio.h>
#include <hardware/structs/io_bank0.h>
#include <pico/error.h>
#include <stdint.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/pwm.h>
#include <stdlib.h>
#include <string.h>
#include "pwm_helpers.h"

#define PIN_LED_RED     17
#define PIN_LED_GREEN   15
#define PIN_LED_BLUE    14

#define COMMAND_BUFFER_MAX  64

static char command_buffer[COMMAND_BUFFER_MAX + 1] = { 0 };

enum Command {
    COMMAND_RED,
    COMMAND_GREEN,
    COMMAND_BLUE,
    COMMAND_INVALID,
    COMMAND_EMPTY
};

static int read_command(void) {
    int ch, command_length = 0;
    while (command_length < COMMAND_BUFFER_MAX - 1) {
        ch = getchar();

        if (ch == '\r') {
            putchar('\n');
            break;
        }

        putchar(ch);
        command_buffer[command_length++] = ch;
    }
    command_buffer[command_length] = '\0';
    return command_length;
}

static int parse_command_name(char *command, int length) {
    int end = 0;
    while (end < length && command[end] != ' ') {
        end++;
    }
    return end;
}

static int parse_command_argument(char *arg_start) {
    // Get the brightness argument:
    char *end;
    int argument = strtol(arg_start, &end, 10);

    // strol failed:
    if (end == arg_start || *end != '\0' || errno == ERANGE)
        return -1;

    return argument;
}

inline static bool validate_argument(int argument) {
    if (argument < 0 || argument > UINT8_MAX)
        return false;

    return true;
}

static enum Command get_command_type(char *command_name, int length) {
    if (length == 0)
        return COMMAND_EMPTY;

    if (strncmp(command_name, "red", length) == 0)
        return COMMAND_RED;
    else if (strncmp(command_name, "green", length) == 0)
        return COMMAND_GREEN;
    else if (strncmp(command_name, "blue", length) == 0)
        return COMMAND_BLUE;
    else
        return COMMAND_INVALID;
}

static void execute_command(enum Command command, uint8_t argument) {
    switch (command) {
        case COMMAND_RED: {
            pwm_write(PIN_LED_RED, argument);
            break;
        }
        case COMMAND_GREEN: {
            pwm_write(PIN_LED_GREEN, argument);
            break;
        }
        case COMMAND_BLUE: {
            pwm_write(PIN_LED_BLUE, argument);
            break;
        }
        default: break;
    }
}

int main() {
    stdio_init_all();

    pwm_init_pin(PIN_LED_RED);
    pwm_init_pin(PIN_LED_GREEN);
    pwm_init_pin(PIN_LED_BLUE);

    while (true) {
        printf("%% ");
        fflush(stdout);

        int command_length = read_command();
        int command_name_length = parse_command_name(command_buffer, command_length);

        enum Command command = get_command_type(command_buffer, command_name_length);
        if (command == COMMAND_EMPTY) 
            continue;
        else if (command == COMMAND_INVALID) {
            printf("Invalid command.\n");
            continue;
        }

        if (command_name_length + 1 >= command_length) {
            printf("Brightness value is required.\n");
            continue;
        }

        int argument = parse_command_argument(&command_buffer[command_name_length + 1]);
        if (!validate_argument(argument)) {
            printf("Brightness value must be between 0 and 255.\n");
            continue;
        }

        execute_command(command, argument);
        sleep_ms(10);
    }
}
