#include "led.h"

#include "p24FJ64GU205.h"

void init_led() {
    ANSCbits.ANSC1 = 0;
    TRISCbits.TRISC1 = 0;
    // pin is wired to bottom of LED
    // init with LED off
    LATCbits.LATC1 = 1;
}

inline void led_on() {
    LATCbits.LATC1 = 0;
}

inline void led_off() {
    LATCbits.LATC1 = 1;
}

inline void led_toggle() {
    LATCbits.LATC1 = ~LATCbits.LATC1;
}