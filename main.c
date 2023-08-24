#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "haw/spm_srxl.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define UART_TX_PIN 8
#define UART_RX_PIN 9
#define UART_ID uart1

#define SRXL2_FRAME_TIMEOUT 22
 // Spektrum channel order
 #define THRO 0
 #define AILE 1
 #define ELEV 2
 #define YAW  3
 #define GEAR 4
 #define AUX1 5
 #define AUX2 6
 #define AUX3 7
 #define AUX4 8
 #define AUX5 9

// UART receive buffer
uint8_t rxBuffer[2 * SRXL_MAX_BUFFER_SIZE];
uint8_t rxBufferIndex = 0;

void userProvidedReceivedChannelData(SrxlChannelData* pChannelData, bool isFailsafeData);

uint32_t millis(void) {
  return to_ms_since_boot(get_absolute_time());
}

int main() {
    stdio_init_all();
    uint32_t current_time;
    uart_init(UART_ID, 115200);
    bool bound = false;
    
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Read data into buffer
    char buffer[128];
    

    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Init the local SRXL device
    if(!srxlInitDevice(SRXL_DEVICE_ID, SRXL_DEVICE_PRIORITY, SRXL_DEVICE_INFO, 0x01000001))
        return 0;

    // Init the SRXL bus: The bus index must always be < SRXL_NUM_OF_BUSES -- in this case, it can only be 0
    if(!srxlInitBus(0, UART_ID, SRXL_SUPPORTED_BAUD_RATES))
        return 0;

    // Sample receive loop -- this is just a simplified example of how you could receive bytes from uart.
    // You might have a much better API for receiving UART bytes with a built-in timeout on your device.
    while(1)
    {
        current_time = millis();
        static uint32_t prev_serial_rx_time = 0;
        if (current_time - prev_serial_rx_time > SRXL2_FRAME_TIMEOUT)
        {
            prev_serial_rx_time = current_time;
            rxBufferIndex = 0;
            srxlRun(0, SRXL2_FRAME_TIMEOUT);
        }

        if (uart_is_readable(UART_ID)){
            prev_serial_rx_time = current_time;
            uint8_t ch = uart_getc(UART_ID);
            rxBuffer[rxBufferIndex++] = ch;
        }

        if(rxBufferIndex < 5) {
            continue;
        }
        
        if(rxBuffer[0] == SPEKTRUM_SRXL_ID)
        {
            uint8_t packetLength = rxBuffer[2];
            
            if(rxBufferIndex >= packetLength)
            {
                // printf("IN - ");
                // for (uint8_t i = 0; i < packetLength; i++){
                //     printf("%X ", rxBuffer[i]);
                // }
                // printf("\n");
                // Try to parse SRXL packet -- this internally calls srxlRun() after packet is parsed and reset timeout
                if(srxlParsePacket(0, rxBuffer, packetLength))
                {                    
                    // Move any remaining bytes to beginning of buffer (usually 0)
                    rxBufferIndex -= packetLength;
                    memmove(rxBuffer, &rxBuffer[packetLength], rxBufferIndex);
                }
                else
                {
                    rxBufferIndex = 0;
                }
            }
        }
        // uint32_t elapsed = millis() - start;
        // printf("Elapsed: %d\n", elapsed);
        // if (elapsed >= 5000 && bound == false){
        //     printf("BINDING");
        //     srxlEnterBind(DSMX_22MS, true);
        //     bound = true;
        // }

    }

    // while (true) {

    //     while (uart_is_readable(UART_ID)){
    //         uint8_t d = uart_getc(UART_ID);
    //         printf("%c", d);
    //     }

    //     gpio_put(LED_PIN, 1);
    //     sleep_ms(250);
    //     gpio_put(LED_PIN, 0);
    //     sleep_ms(250);
    // }
}

// User-defined routine to use the provided channel data from the SRXL bus master
void userProvidedReceivedChannelData(SrxlChannelData* pChannelData, bool isFailsafeData)
{
    // Use the received channel data in whatever way desired.
    // The failsafe flag is set if the data is failsafe data instead of normal channel data.
    // You can directly access the last received packet data through pChannelData,
    // in which case the values are still packed into the beginning of the array of channel data,
    // and must be identified by the mask bits that are set (NOT recommended!).
    // This is mostly provided in case you want to know which channels were actually sent
    // during this packet:
    uint8_t currentIndex = 0;
    printf("THRO - %d ", srxlChData.values[THRO] >> 6);
    printf("AILE - %d ", srxlChData.values[AILE] >> 6);
    printf("ELEV - %d ", srxlChData.values[ELEV] >> 6);
    printf("YAW - %d\n", srxlChData.values[YAW] >> 6);
}

void uartTransmit(uint8_t uart, uint8_t* pBuffer, uint8_t length)
 {
    // printf("OUT - ");
    for (uint8_t i=0; i < length; i++)
    {
        // printf("%02X ", rxBuffer[i]);
        uart_putc(UART_ID, pBuffer[i]);
    }
    // printf("\n");
 }

// openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program spm4650.elf verify reset exit"