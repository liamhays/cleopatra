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
#include <float.h>

#include "minmea/minmea.h"



#include "led.h"
#include "debug.h"
#include "gps.h"
#include "lcd.h"
/*****
 **** PINS ****
 * UART1 (GPS at 9600 baud): RX into RC5 = RP21
 *   This doesn't seem to interfere with anything, the way some pins do
 * UART2 (Debug at 115200 baud): RX into RA11, TX out of RC9
 * LCD: D4-D7 = RA7-RA10, RS = RB8, EN = RB9
 * SPI: MOSI on RC3, MISO on RC4, SCK on RC8, SS on RB7
 * user button: RA11 to ground, so enable pullup
****/


// location calculation functions from TinyGPS++ arduino library
#define _GPS_EARTH_MEAN_RADIUS 6371009
#define PI (3.14159)
float earth_distance_between(float lat1, float long1, float lat2, float long2) {
  // returns distance in meters between two positions, both specified
  // as signed decimal-degrees latitude and longitude. Uses great-circle
  // distance computation for hypothetical sphere of radius 6371009 meters.
  // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
  // Courtesy of Maarten Lamers
  float delta = (long1-long2)*PI / 180; // to radians
  float sdlong = sin(delta);
  float cdlong = cos(delta);
  lat1 = lat1*PI / 180;
  lat2 = lat2*PI / 180;
  float slat1 = sin(lat1);
  float clat1 = cos(lat1);
  float slat2 = sin(lat2);
  float clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
  delta = (delta*delta);
  delta += (clat2 * sdlong) * (clat2 * sdlong);
  delta = sqrt(delta);
  float denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
  delta = atan2(delta, denom);
  return delta * _GPS_EARTH_MEAN_RADIUS;
}


float heading_to_location(float lat1, float long1, float lat2, float long2) {
  // returns course in degrees (North=0, West=270) from position 1 to position 2,
  // both specified as signed decimal-degrees latitude and longitude.
  // Because Earth is no exact sphere, calculated course may be off by a tiny fraction.
  // Courtesy of Maarten Lamers
  float dlon = (long2-long1) * PI / 180;
  lat1 = lat1*PI / 180;
  lat2 = lat2*PI / 180;
  float a1 = sin(dlon) * cos(lat2);
  float a2 = sin(lat1) * cos(lat2) * cos(dlon);
  a2 = cos(lat1) * sin(lat2) - a2;
  a2 = atan2(a1, a2);
  if (a2 < 0.0)
  {
    a2 += 2*PI;
  }
  return (a2/180)*PI;
}


typedef enum CAESAR_SEEKER_SYSTEM_STATE {
    WAITING_FOR_FIX = 0,
    SEARCHING = 1,
    NAVIGATING = 2,
} caesar_seeker_system_state_t;

typedef enum NAVIGATION_DISPLAY_MODE {
    STREET_ADDRESS_PAGE_1 = 0,
    STREET_ADDRESS_PAGE_2 = 1,        
    DISTANCE_TO_LOCATION = 2,
    NAVIGATION_DISPLAY_MODE_END,
            
} navigation_display_mode_t;
// all of this code assumes that the GPS is sending the RMC and the GGA message!



typedef struct little_caesars_location {
    uint32_t location_number;
    float latitude;
    float longitude;
    char street_address[128];
} little_caesars_location_t;


// data format: 
// - 4 bytes (u32) location number
// - 4 bytes latitude (float32)
// - 4 bytes longitude (float32)
// - 128 bytes null-terminated street address

#define LOCATION_DATABASE_ENTRY_LENGTH (4+4+4+128)
// 30 stores is 4440 bytes, which is fine
#define LOCATION_COUNT_IN_BUFFER (30)
#define FLASH_BUFFER_BYTES (LOCATION_DATABASE_ENTRY_LENGTH*LOCATION_COUNT_IN_BUFFER)
uint8_t flash_buffer[FLASH_BUFFER_BYTES];
uint32_t flash_address = 0;

// used in debug.c
extern bool global_sending_debug_message;
// used in gps.c
extern bool global_gps_nmea_message_complete;
extern uint8_t global_gps_uart_buffer[];

// double doesn't exist! it gets converted to binary32 float
float dtest = 52.2;
little_caesars_location_t location_info = {0};
little_caesars_location_t nearest_location = {0};

    
uint16_t last_timer_value = 0;

// printing \r\n stops minicom from buffering on linux, for some reason
int main(void) {
    init_debug_uart();
    init_gps_uart();
    init_led();
    init_lcd();
    init_spi();

    // init RA11 button
    TRISAbits.TRISA11 = 1; // input

    // PIC24 is little-endian
    
    // use timer 1 for debouncing
    // set to 1:256 prescale (15625Hz)
    T1CONbits.TCKPS = 0b11;
    T1CONbits.TON = 1;
    
    memset(flash_buffer, 0, sizeof(flash_buffer));
    
    uint32_t stores_count = 0;
    read_spi_flash(flash_address, 4, &stores_count);
    
    flash_address += 4;
    
    char gps_message[MAX_GPS_CHARS];
    caesar_seeker_system_state_t current_system_state = WAITING_FOR_FIX;
    navigation_display_mode_t current_navigation_display_mode = STREET_ADDRESS_PAGE_1;
    char lcd_string[32];
    float current_latitude = 0;
    float current_longitude = 0;
    float current_heading = 0;
    float search_latitude = 0;
    float search_longitude = 0;
    float nearest_store_distance = FLT_MAX;
    
    uint32_t locations_searched = 0;
    uint8_t current_fix_type = 0;
    
    lcd_home();
    lcd_print("waiting for fix");
    while (1) {
        if (global_gps_nmea_message_complete) {
            // copy the message to our own buffer so we can do stuff with it
            memcpy(gps_message, global_gps_uart_buffer, MAX_GPS_CHARS);
            
            global_gps_nmea_message_complete = false;
            
            struct minmea_sentence_rmc rmc_frame = {0};
            struct minmea_sentence_gga gga_frame = {0};
        
            switch (minmea_sentence_id(gps_message, false)) {
                case MINMEA_SENTENCE_RMC:
                    // just ignore messages that don't parse
                    // some of the messsages on printf don't look right,
                    // but I can't tell if that's a GPS receiving issue or
                    // a printf issue.
                    if (minmea_parse_rmc(&rmc_frame, gps_message)) {
                        current_latitude = minmea_tocoord(&rmc_frame.latitude);
                        current_longitude = minmea_tocoord(&rmc_frame.longitude);
                        current_heading = minmea_tofloat(&rmc_frame.course);
                    }

                    break;

                case MINMEA_SENTENCE_GGA:
                    if (minmea_parse_gga(&gga_frame, gps_message)) {
                        current_fix_type = gga_frame.fix_quality;
                    }
                    break;
                default:
                    // don't care about any other messages
                    break;
                    
            }
        }

        // reference manual says this bit stops data reception if left at 1
        if (U1STAbits.OERR == 1) {
            debug_printf("UART1 OERR\r\n");
            U1STAbits.OERR = 0;
        }
        
        // run state machine
        if (current_system_state == WAITING_FOR_FIX) {
            // update display
            // if the GPS has a 3D fix, the position data is usable and we
            // can go search the database
            if (current_fix_type == 1 || current_fix_type == 2) {
                // 1: normal fix
                // 2: fix with differential data
                lcd_home();
                lcd_print("GPS fix acquired");
                // capture the latitude and longitude for the search
                search_latitude = current_latitude;
                search_longitude = current_longitude;
                locations_searched = 0;
                current_system_state = SEARCHING;
            } 
            
        } else if (current_system_state == SEARCHING) {
            // search through the flash database
            for (uint32_t i = 0; i < stores_count / LOCATION_COUNT_IN_BUFFER; i++) {
                lcd_set_cursor(0, 1);
                char str[16];
                memset(str, 0, sizeof(str));
                sprintf(str, "search %lu/%lu", i*LOCATION_COUNT_IN_BUFFER, stores_count);
                lcd_print(str);
                
                read_spi_flash(flash_address, FLASH_BUFFER_BYTES, flash_buffer);
                flash_address += FLASH_BUFFER_BYTES;

                for (uint32_t store = 0; store < LOCATION_COUNT_IN_BUFFER; store++) {
                    // copy into the location struct
                    memcpy(&(location_info.location_number), &flash_buffer[store*LOCATION_DATABASE_ENTRY_LENGTH], 4);
                    // byte order is backward on lat/lon even though it works on other platforms
                    memcpy(&location_info.latitude, &flash_buffer[store*LOCATION_DATABASE_ENTRY_LENGTH+4], 4);
                    memcpy(&location_info.longitude, &flash_buffer[store*LOCATION_DATABASE_ENTRY_LENGTH+4+4], 4);
                    memcpy(&location_info.street_address, &flash_buffer[store*LOCATION_DATABASE_ENTRY_LENGTH+4+4+4], 128);

                    float distance = earth_distance_between(search_latitude, search_longitude, location_info.latitude, location_info.longitude);
                    if (distance < nearest_store_distance) {
                        // copy location info
                        memcpy(&nearest_location, &location_info, sizeof(location_info));
                        nearest_store_distance = distance;
                    }
                }
            }
            // if we get here, we're done searching
            debug_printf("nearest location is store #%lu, distance %f, addr %s\r\n", 
                    nearest_location.location_number, nearest_store_distance, nearest_location.street_address);
            __delay_ms(50);
            current_system_state = NAVIGATING;
            
        } else if (current_system_state == NAVIGATING) {
            // I couldn't get interrupts to work, but the poll rate of 
            // this loop is plenty fast for this to work well
            if (!PORTAbits.RA11 && abs(TMR1 - last_timer_value) > 5000) {
                // note the debouncing
                lcd_clear();
                current_navigation_display_mode++;
                if (current_navigation_display_mode == NAVIGATION_DISPLAY_MODE_END) {
                    current_navigation_display_mode = 0;
                }   
                last_timer_value = TMR1;
            }
            
            if (current_navigation_display_mode == STREET_ADDRESS_PAGE_1) {
                lcd_home();
                lcd_print(nearest_location.street_address);
                lcd_set_cursor(0, 1);
                lcd_print(&nearest_location.street_address[16]);
            } else if (current_navigation_display_mode == STREET_ADDRESS_PAGE_2) {
                if (strlen(nearest_location.street_address) <= 32) {
                    current_navigation_display_mode = DISTANCE_TO_LOCATION;
                    continue;
                }
                
                lcd_home();
                lcd_print(&nearest_location.street_address[32]);
                lcd_set_cursor(0, 1);
                lcd_print(&nearest_location.street_address[48]);
            } else if (current_navigation_display_mode == DISTANCE_TO_LOCATION) {
                float distance = earth_distance_between(current_latitude, current_longitude,
                        nearest_location.latitude, nearest_location.longitude);
                lcd_home();
                char str[16];
                memset(str, 0, sizeof(str));
                sprintf(str, "%lu m away", (uint32_t)distance);
                lcd_print(str);
                float heading = heading_to_location(current_latitude, current_longitude,
                        nearest_location.latitude, nearest_location.longitude);
                memset(str, 0, sizeof(str));
                lcd_set_cursor(0, 1);
                sprintf(str, "heading %u", (uint16_t)heading);
                lcd_print(str);
                lcd_print_char(223); // degree symbol
            }
        }
        
    }
    

}
