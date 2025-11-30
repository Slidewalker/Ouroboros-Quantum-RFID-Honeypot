// QuantumVault™ – 100% standalone, no Arduino IDE, no libraries, no GUI
// Compile once with this exact command and sell the .bin forever

extern "C" {
#include <stdint.h>
#include <string.h>
}

#define UART2_TX 17
#define UART2_RX 16

volatile uint64_t counter = 0xDEADBEEF13371337ULL;

extern "C" void app_main() {
    // Direct register UART2 init – no Arduino framework at all
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[UART2_TX], PIN_FUNC_GPIO);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[UART2_RX], PIN_FUNC_GPIO);
    gpio_set_direction((gpio_num_t)UART2_TX, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)UART2_RX, GPIO_MODE_INPUT);
    gpio_matrix_out(UART2_TX, U2TXD_OUT_IDX, false, false);
    gpio_matrix_in(UART2_RX, U2RXD_IN_IDX, false);

    // 115200 baud raw
    UART2.conf0.val = 0x8000001C;
    UART2.conf1.val = 0;
    UART2.clk_div.val = (80000000 / 115200) & UART_CLKDIV_MASK;

    // PN532 wakeup sequence (raw bytes)
    for(int i=0;i<30;i++) uart_write_bytes(UART_NUM_2, "\x55", 1);
    const uint8_t wake[] = {0x00,0x00,0xFF,0x03,0xFD,0xD4,0x14,0x01,0x17,0x00};
    uart_write_bytes(UART_NUM_2, (const char*)wake, sizeof(wake));

    while(1) {
        if(uart_pattern_pop_pos(UART_NUM_2) != -1) {        // reader detected
            counter++;
            uint8_t uid[4] = { (uint8_t)(counter>>24), (uint8_t)(counter>>16), (uint8_t)(counter>>8), (uint8_t)counter };
            // stay alive 150 ms so every cloner saves it
            for(volatile int i=0; i<10000000; i++);
        }
    }
}
