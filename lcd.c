// HD44780 library
// adapted from Arduino LiquidCrystal

#include "lcd.h"
#include "debug.h"

// https://support.microchip.com/s/article/Delay-functions-in-XC16---XC8-using-PIC-devices
#define FCY (4000000)
#include <libpic30.h>
#include <xc.h>

#include "p24FJ64GU205.h"

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

// functions marked inline don't link?

void set_rs_high() {
    LATBbits.LATB8 = 1;
}

void set_rs_low() {
    LATBbits.LATB8 = 0;
}

// send a pulse on EN
void pulse_en() {
    LATBbits.LATB9 = 0;
    __delay_us(1);
    LATBbits.LATB9 = 1;
    __delay_us(1);
    LATBbits.LATB9 = 0;
    __delay_us(100);
}

// set the data lines to the low 4 bits of `bits`,
// with the lowest bit on RA7
void write_4_bits(uint8_t bits) {
    LATAbits.LATA7 = bits & 0x1;
    LATAbits.LATA8 = (bits >> 1) & 0x1;
    LATAbits.LATA9 = (bits >> 2) & 0x1;
    LATAbits.LATA10 = (bits >> 3) & 0x1;
    pulse_en();
}

void send_byte(uint8_t value, bool is_data) {
    // if data, RS needs to be 1
    if (is_data) {
        set_rs_high();
    } else {
        set_rs_low();
    }
    
    // first data sent is the high bits
    write_4_bits(value >> 4);
    write_4_bits(value);
    
}


// pins: LCD: D4-D7 = RA7-RA10, RS = RB8, EN = RB9
void init_lcd() {

    // LCD datasheet says the chip needs like 50ms to start up
    __delay_ms(50);
    // set to digital (first five don't have analog modes)
    /*ANSAbits.ANSA7 = 0;
    ANSAbits.ANSA8 = 0;
    ANSAbits.ANSA9 = 0;
    ANSAbits.ANSA10 = 0;
    ANSBbits.ANSB8 = 0;*/
    ANSBbits.ANSB9 = 0;
    
    // set to output
    TRISAbits.TRISA7 = 0;
    TRISAbits.TRISA8 = 0;
    TRISAbits.TRISA9 = 0;
    TRISAbits.TRISA10 = 0;
    TRISBbits.TRISB8 = 0;
    TRISBbits.TRISB9 = 0;
    
    // set all low 
    LATAbits.LATA7 = 0;
    LATAbits.LATA8 = 0;
    LATAbits.LATA9 = 0;
    LATAbits.LATA10 = 0;
    LATBbits.LATB8 = 0;
    LATBbits.LATB9 = 0;
    
    // with the pins enabled, we can configure the LCD driver
    // this is exactly how the driver says to do it
    write_4_bits(0x03);
    __delay_us(4500);
    write_4_bits(0x03);
    __delay_us(4500);
    write_4_bits(0x03);
    __delay_us(150);
    // set to 4-bit interface
    write_4_bits(0x02);
    
    // set display "function" (mode, I guess)
    uint8_t display_function = LCD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
    
    send_byte(display_function, false);
    
    // Turn display on, no cursor
    uint8_t display_control = LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    
    send_byte(display_control, false);
    
    // clear display
    send_byte(LCD_CLEARDISPLAY, false);
    // needs 2ms to execute
    __delay_ms(2);
    
    // set left-to-right text
    uint8_t display_mode = LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

    send_byte(display_mode, false);
    
    debug_printf("display_function: %x, display_control: %x, display_mode: %x\r\n",
            display_function, display_control, display_mode);
    __delay_ms(5);
}

void lcd_print(const char* string) {
    for (int i = 0; i < strlen(string); i++) {
        if (i == 16) {
            // display has 16 chars per line and doesn't wrap
            return;
        }
        
        send_byte(string[i], true);
    }
}

void lcd_home() {
    send_byte(LCD_RETURNHOME, false);
    __delay_ms(2);
}