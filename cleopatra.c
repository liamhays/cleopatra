/*
 * File:   cleopatra.c
 * Author: liam
 *
 * Created on October 24, 2026, 8:22 PM
 */


// PIC24FJ64GU205 Configuration Bit Settings

// 'C' source line config statements

// FSEC
#pragma config BWRP = OFF               // Boot Segment Write-Protect bit (Boot Segment may be written)
#pragma config BSS = DISABLED           // Boot Segment Code-Protect Level bits (No Protection (other than BWRP))
#pragma config BSEN = OFF               // Boot Segment Control bit (No Boot Segment)
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = DISABLED           // General Segment Code-Protect Level bits (No Protection (other than GWRP))
#pragma config CWRP = OFF               // Configuration Segment Write-Protect bit (Configuration Segment may be written)
#pragma config CSS = DISABLED           // Configuration Segment Code-Protect Level bits (No Protection (other than CWRP))
#pragma config AIVTDIS = OFF            // Alternate Interrupt Vector Table bit (Disabled AIVT)

// FBSLIM
#pragma config BSLIM = 0x1FFF           // Boot Segment Flash Page Address Limit bits (Enter Hexadecimal value)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Source Selection (Internal Fast RC (FRC))
#pragma config PLLMODE = DISABLED       // PLL Mode Selection (No PLL used; PLLEN bit is not available)
#pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Mode Select bits (Primary Oscillator disabled)
#pragma config OSCIOFCN = OFF           // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config SOSCSEL = OFF            // SOSC Selection Configuration bits (Digital (SCLKI) mode)
#pragma config PLLSS = PLL_FRC          // PLL Secondary Selection Configuration bit (PLL is fed by the on-chip Fast RC (FRC) oscillator)
#pragma config IOL1WAY = ON             // Peripheral pin select configuration bit (Allow only one reconfiguration)
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits (Clock switching is enabled,Fail-safe Clock Monitor is disabled)

// FWDT
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler bits (1:32,768)
#pragma config FWPSA = PR128            // Watchdog Timer Prescaler bit (1:128)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bits (WDT and SWDTEN disabled)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config WDTWIN = WIN25           // Watchdog Timer Window Select bits (WDT Window is 25% of WDT period)
#pragma config WDTCMX = WDTCLK          // WDT MUX Source Select bits (WDT clock source is determined by the WDTCLK Configuration bits)
#pragma config WDTCLK = LPRC            // WDT Clock Source Select bits (WDT uses LPRC)

// FPOR
#pragma config BOREN = ON               // Brown Out Enable bit (Brown-out Reset is Enabled )
#pragma config LPREGEN = OFF            // Low power regulator control (Low Voltage and Low Power Regulator are not available)
#pragma config LPBOREN = ENABLE         // Downside Voltage Protection Enable bit (Low Power BOR is enabled and active when main BOR is inactive)

// FICD
#pragma config ICS = PGD3               // ICD Communication Channel Select bits (Communicate on PGEC3 and PGED3)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FDMTIVTL
#pragma config DMTIVTL = 0xFFFF         // Deadman Timer Interval Low Word (Enter Hexadecimal value)

// FDMTIVTH
#pragma config DMTIVTH = 0xFFFF         // Deadman Timer Interval High Word (Enter Hexadecimal value)

// FDMTCNTL
#pragma config DMTCNTL = 0xFFFF         // Deadman Timer Instruction Count Low Word (Enter Hexadecimal value)

// FDMTCNTH
#pragma config DMTCNTH = 0xFFFF         // Deadman Timer Instruction Count High Word (Enter Hexadecimal value)

// FMDT
#pragma config DMTDIS = OFF             // Deadman Timer Enable Bit (Dead Man Timer is Disabled and can be enabled by software)

// FDEVOPT1
#pragma config ALTCMP1 = DISABLE        // Alternate Comparator 1 Input Enable bit (C1INC is on RB13 and C3INC is on RA0 )
#pragma config TMPRPIN = OFF            // Tamper Pin Enable bit (TMPRN pin function is disabled)
#pragma config SOSCHP = ON              // SOSC High Power Enable bit (valid only when SOSCSEL = 1 (Enable SOSC high power mode (default))
#pragma config ALTI2C1 = ALTI2C1_OFF    // Alternate I2C pin Location (I2C1 Pin mapped to SDA1/SCL1 pins)
#pragma config ALTCMP2 = DISABLE        // Alternate Comparator 2 Input Enable bit (C2INC is on RA4 and C2IND is on RB4)
#pragma config SMB3EN = SMBUS3          // SM Bus Enable (SMBus 3.0 input levels)


// https://support.microchip.com/s/article/Delay-functions-in-XC16---XC8-using-PIC-devices
#define FCY (4000000)
#include <libpic30.h>


#include <xc.h>

#include "p24FJ64GU205.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "minmea/minmea.h"



#include "led.h"
#include "debug.h"
#include "gps.h"
#include "lcd.h"
/*****
 **** PINS ****
 * UART1 (GPS at 9600 baud): RX into RB0, TX out of RB1
 * UART2 (Debug at 115200 baud): RX into RA11, TX out of RC9
 * LCD: D4-D7 = RA7-RA10, RS = RB8, EN = RB9
 * SPI: MOSI on RB8, MISO on RC4, SCK on RC5, SS on RB7
****/

typedef enum CAESAR_SEEKER_SYSTEM_STATE {
    WAITING_FOR_FIX = 0,
    SEARCHING = 1,
    NAVIGATING = 2,
} caesar_seeker_system_state_t;

void init_spi() {
    
}

void read_flash(void* location, uint16_t number_of_bytes) {
    
}

// all of this code assumes that the GPS is sending the RMC and GGA message!
// RMC includes PVT and fix status, GGA includes


typedef struct little_caesars_location {
    uint32_t location_number;
    double latitude;
    double longitude;
    char street_address[128];
} little_caesars_location_t;

// data format: 
// - 4 bytes (u32) location number
// - 8 bytes latitude (float64)
// - 8 bytes longitude (float64)
// - 128 bytes null-terminated street address

#define LOCATION_DATABASE_ENTRY_LENGTH (4+8+8+128)
// 30 stores is 4440 bytes, which is fine
#define LOCATION_COUNT_IN_BUFFER (30)
uint8_t flash_buffer[LOCATION_DATABASE_ENTRY_LENGTH*LOCATION_COUNT_IN_BUFFER];

// used in debug.c
extern bool global_sending_debug_message;




int main(void) {
    init_debug_uart();
    init_gps_uart();
    init_led();
    //init_lcd();
    init_lcd();
    char str[16];
    sprintf(str, "%g", sin(1.2345));
    lcd_print(str);//"Big chungus is among us.");
    while (1) {
        /*if (!global_sending_debug_message) {
            debug_printf("Hello world!\r\n");
        }*/
        
        // reference manual says this bit stops data reception if left at 1
        if (U1STAbits.OERR == 1) {
            U1STAbits.OERR = 0;
        }
        
    }
    
    //while (1);
    /*while (1) {
        U2TXREG = 'a';
        asm volatile ("REPEAT, #4201"); Nop();
    }*/
    //debug_printf("%s", "Hello world!");
    /*
    caesar_seeker_system_state_t current_system_state = WAITING_FOR_FIX;
    uint32_t flash_pointer = 0;
    
    // scaled to 3 decimal places by minmea
    // 
    // 
    float current_longitude = 0;
    float current_latitude = 0;
    little_caesars_location_t nearest_location = {0};
    
    // we want to capture the current lat/lon for the search
    
    float captured_search_longitude = 0;
    float captured_search_latitude = 0;
    
    uint32_t total_location_count = 0;
    uint32_t locations_searched = 0;
    
    enum minmea_gsa_fix_type fix_type = 0;
    
    struct minmea_sentence_rmc rmc_frame;
    struct minmea_sentence_gsa gsa_frame;
    // basic loop logic: check for new GPS data, then update the state machine
    while (true) {
        // if this flag is true, we have a complete sentence
        // (the interrupt handler will clear the flag)
        // if the last character was the terminator (linefeed), reset the GPS UART
        // buffer counter
        if (global_gps_nmea_message_complete) {
            // if the latest message was completed, reset the counter to start
            // writing over (good practice will have the mainloop watch this flag)
            // and quickly copy the buffer when the flag is set
            //
            // clear the flag
            global_gps_nmea_message_complete = false;

            // and clear the current gps string buffer counter
            global_gps_uart_counter = 0;        

            // no idea what setting the second parameter (`strict`) to false
            // does, but the example does it
            switch (minmea_sentence_id((const char*)global_gps_uart_buffer, false)) {
                // rmc (PVT) has coordinates
                case MINMEA_SENTENCE_RMC:
                    if (minmea_parse_rmc(&rmc_frame, (const char*)global_gps_uart_buffer)) {
                        longitude = rmc_frame.longitude;
                        latitude = rmc_frame.latitude;
                    }

                    break;
                // gga has fix status
                case MINMEA_SENTENCE_GSA:
                    if (minmea_parse_gsa(&gsa_frame, (const char*)global_gps_uart_buffer)) {
                        // update fix status
                        fix_type = gsa_frame.fix_type;
                    }
                    break;
                default:
                    // don't care about any other messages
                    // this also suppresses compiler warnings
                    break;   
            }
        }

        // this only executes when we have a 3D fix
        if (current_system_state == WAITING_FOR_FIX) {
            // update display
            // if the GPS has a 3D fix, the position data is usable and we
            // can go search the database
            if (fix_type == MINMEA_GPGSA_FIX_3D) {
                flash_pointer = 0;
                // capture the latitude and longitude for the search
                captured_search_latitude = current_latitude;
                captured_search_longitude = current_longitude;
                locations_searched = 0;
                current_system_state = SEARCHING;
            } 
            
        } else if (current_system_state == SEARCHING) {
            // search through the flash database
            // start by reading the 4-byte number of store locations
            if (locations_searched == 0) {
                // if we just got here (locations_searched is 0),
                // load in the total number of locations in the database
                read_flash(&number_of_locations, 4);
                flash_pointer += 4;
                memset(flash_buffer, 0, sizeof(flash_buffer));
            }
            
            // fill the buffer
            read_flash(&flash_buffer, sizeof(flash_buffer));
            flash_pointer += sizeof(flash_buffer);
            for (int i = 0; i < LOCATION_COUNT_IN_BUFFER; i++) {
                // get the latitude and longitude
                double this_location_latitude = 
                  (double)flash_buffer[i*LOCATION_DATABASE_ENTRY_LENGTH+4];
                double this_location_longitude =
                  (double)flash_buffer[i*LOCATION_DATABASE_ENTRY_LENGTH+4+8];
                i
                locations_searched++;
            }
        }

        */

}
