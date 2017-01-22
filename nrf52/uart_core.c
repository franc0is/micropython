#include <unistd.h>
#include "py/mpconfig.h"
#include "app_uart.h"

/*
 * Core UART functions to implement for a port
 */

// Receive single character
int mp_hal_stdin_rx_chr(void) {
    unsigned char c = 0;
#if MICROPY_MIN_USE_STDOUT
    int r = read(0, &c, 1);
#elif MICROPY_MIN_USE_NRF52_MCU
    app_uart_get(&c);
#endif
    return c;
}

// Send string of given length
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
#if MICROPY_MIN_USE_STDOUT
    int r = write(1, str, len);
#elif MICROPY_MIN_USE_NRF52_MCU
    while (len--) {
        // wait for TXE
	app_uart_put(*str++);
    }
#endif
}
