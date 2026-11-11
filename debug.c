#include "debug.h"

#include "led.h"

#include <xc.h>

#include "p24FJ64GU205.h"

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

bool global_sending_debug_message = false;
uint16_t global_debug_message_length = 0;
char global_debug_print_buffer[MAX_DEBUG_CHARS]; 
uint16_t global_debug_message_current_index = 0;

// messages that come out longer than MAX_DEBUG_CHARS will be 
void debug_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    // if we aren't currently printing a message, we can send a new one
    if (!global_sending_debug_message) {
        memset(global_debug_print_buffer, 0, sizeof(global_debug_print_buffer));
        // print to the buffer according to `format` and `args`
        global_debug_message_length = vsnprintf(global_debug_print_buffer, 
                MAX_DEBUG_CHARS, format, args);
        // buffer is now ready for sending
        // 
        // the microchip recommended way to do this is to send a new character
        // every time we get the UART TX interrupt, so reset all the interrupt variables
        // for this new message
        global_sending_debug_message = true;
        global_debug_message_current_index = 0;
        
        // interrupt will send the rest of the buffer
        U2TXREG = global_debug_print_buffer[global_debug_message_current_index];
        // for some reason, the first character gets gobbled sometimes (maybe the fault
        // of the debugger?). simply not incrementing the index fixes this
        // by sending the first character twice.
        global_debug_message_current_index++;
    }
    va_end(args);
}


// enable debug UART on UART2, mapped to pins RA11 = RPIN29 = RX and RC9 = RP25 = TX at 115200 baud
// those pins go to the CDC UART in the programmer
void init_debug_uart() {
    RPINR19bits.U2RXR = 29;
    RPOR12bits.RP25R = 5; // 5 = UART2 transmit
    
    // configure to standard UART: 1 stop bit, no parity, 8-bit bytes
    U2MODEbits.STSEL = 0; // 1 stop bit
    U2MODEbits.PDSEL = 0; // no parity, 8 bits
    U2MODEbits.ABAUD = 0; // no autobaud
    U2MODEbits.BRGH = 0; // no high baud rate
    
    U2BRG = DEBUG_BRGVAL; // set UART2 to 115200 baud
    
    // this UART used for transmit only (receive would mean scanf
    // and whatever else, which is a lot of work and we are LAZY)
    
    // enable UART2 TX interrupts
    IEC1bits.U2TXIE = 1;
    
    U2MODEbits.UARTEN = 1;
    // enable TX
    U2STAbits.UTXEN = 1;
    
    // wait at least 1 byte's time before sending
    
    // this is from the reference manual and is set for 9600 baud,
    // so it's enough time
   
    asm volatile ("REPEAT, #4201"); Nop();
    
}

// it's a little confusing to put the interrupt handler in here,
// but it's relatively clean

// no_auto_psv indicates that the interrupt handler does not touch
// the PSV functionality
void __attribute__((__interrupt__,auto_psv)) _U2TXInterrupt() {
    IFS1bits.U2TXIF = 0; // clear interrupt
    
    // if there's a debug message being sent, continue sending it
    if (global_sending_debug_message) {
        U2TXREG = global_debug_print_buffer[global_debug_message_current_index]; // send next character
        global_debug_message_current_index++;
        
        // if we're done sending the message (index increments to total length), clear the bool flag
        if (global_debug_message_current_index == global_debug_message_length) {
            global_sending_debug_message = false;
        }
    }
}
