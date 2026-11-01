#include "gps.h"

#include <xc.h>

#include "p24FJ64GU205.h"

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

// see https://en.wikipedia.org/wiki/NMEA_0183 for info on NMEA format

uint8_t global_gps_uart_buffer[MAX_GPS_CHARS];
uint16_t global_gps_uart_counter = 0;
bool global_gps_nmea_message_complete = false;



// NEO-M6 series defaults to 9600 baud

// configure UART1 to 9600 baud, 8 bits, 1 start bit, on pins
void init_gps_uart() {
    // pins are already unlocked by default
    
    // set UART1 RX pin to RP0 (RB0) and UART1 TX pin to RP1 (RB1)
    RPINR18bits.U1RXR = 0;
    RPOR0bits.RP1R = 3; // 3 = UART1 transmit
    
    // configure UART to the standard default: 1 stop bit, no parity,
    // 8-bit bytes, and set it to the GPS's 9600 baud
    U1MODEbits.STSEL = 0; // 1 stop bit
    U1MODEbits.PDSEL = 0; // no parity, 8 bits
    U1MODEbits.ABAUD = 0; // disable autobaud (requires a 0x55 byte which is unlikely)
    U1MODEbits.BRGH = 0; // normal speed is sufficient
    
    U1BRG = GPS_BRGVAL; // set UART1 to 9600 baud
    
    // this UART is only used for listening, so we enable receive
    U1STAbits.URXEN = 1;

    // interrupt every time the UART receive buffer has 1 or more 
    // characters
    U1STAbits.URXISEL = 0;
    
    // and enable UART1 receive interrupts
    IEC0bits.U1RXIE = 1;
    
    // and enable the UART
    U1MODEbits.UARTEN = 1;
}


// same as with debug, we put the interrupt handler here.
void __attribute__((__interrupt__,auto_psv)) _U1RXInterrupt() {
    // clear the interrupt
    IFS0bits.U1RXIF = 0;

    // on GPS UART interrupt, write the new byte to the GPS UART buffer
    //
    // we're relying on the mainloop to handle the flag clearing
    
    
    // write the current byte
    global_gps_uart_buffer[global_gps_uart_counter] = U1RXREG;
    // increment the counter
    global_gps_uart_counter++;
    
    // if we just got a linefeed (end delimiter), the message is over
    if (global_gps_uart_buffer[global_gps_uart_counter] == 0x0a) {
        global_gps_nmea_message_complete = true;
    }
}
