#include "spi.h"

#include "debug.h"

// https://support.microchip.com/s/article/Delay-functions-in-XC16---XC8-using-PIC-devices
#define FCY (4000000)
#include <libpic30.h>
#include <xc.h>

#include "p24FJ64GU205.h"


// annoying interface: SPI1BUF maps to different internal registers
// depending on whether you're reading or writing


void assert_chip_select() {
    LATBbits.LATB7 = 0;
}

void deassert_chip_select() {
    LATBbits.LATB7 = 1;
}

void init_spi() {
    // 8-bit mode
    
    // set pins:
    // MOSI on RC3 = RP19, MISO on RC4 = RP20, SCK on RC8 = RP24, SS on RB7 = RP7
    TRISCbits.TRISC4 = 1; // input MOSI (needed?)
    TRISCbits.TRISC8 = 0; // output SCK
    RPINR20bits.SDI1R = 20; // MISO
    
    RPOR9bits.RP19R = 7; // MOSI
    RPOR12bits.RP24R = 8; // SCK
    
    // no chip select builtin to the SPI peripheral, so we drive it manually
    TRISBbits.TRISB7 = 0;
    deassert_chip_select();
    
    // following host mode setup in the SPI manual
    
    // disable SPI interrupts
    IEC0bits.SPI1TXIE = 0;
    IEC0bits.SPI1IE = 0;
    IEC3bits.SPI1RXIE = 0;
    
    // stop the SPI module
    SPI1CON1Lbits.SPIEN = 0;
    
    // clear the receive buffer (?)
    
    // clear ENHBUF bit for standard buffer mode
    SPI1CON1Lbits.ENHBUF = 0;
    
    // clear SPI interrupt bits (we want RX interrupts only)
    IFS0bits.SPI1TXIF = 0;
    IFS0bits.SPI1IF = 0;
    IFS3bits.SPI1RXIF = 0;
    // default priority is fine
    IEC3bits.SPI1RXIE = 0;
    
    SPI1BRGL = SPI_BRG_VALUE;
    // clear SPIROV bits
    SPI1STATLbits.SPIROV = 0;
    
    // set host mode
    SPI1CON1Lbits.MSTEN = 1;
    
    // flash will use SPI mode 0
    SPI1CON1Lbits.CKP = 0; // clock polarity 0 (low = idle)
    // REQUIRED for SPI flash to work!!
    // this matches Arduino SPI default. the PIC24 default 
    // sets data on SCK rising edges, and likely MOSI doesn't
    // get set fast enough for the flash to read it on the clock.
    
    // (this seems to be different from the standard SPI CPHA parameter?
    // CPHA controls when sampling occurs, and this controls when transmit
    // occurs. the SMP bit controls when sampling occurs)
    SPI1CON1Lbits.CKE = 1;
    // other bits are probably fine
    
    // set word length to 8 bits
    SPI1CON1Lbits.MODE = 0; // 32-deep FIFO
    
    // enable interrupt when receive buffer fills
    SPI1IMSKLbits.SPIRBFEN = 1;
    
    // enable SPI1
    SPI1CON1Lbits.SPIEN = 1;
}

// from https://github.com/muradxq/PIC24-SPI-Communication/blob/master/SPI_Master.c
// this actually works, unlike the interrupt thing I was trying
uint8_t transfer_spi_byte(uint8_t b) {
    SPI1BUFL = b;
    
    // wait for data in receive buffer
    while (!SPI1STATLbits.SPIRBF) ;
    
    // get byte
    return SPI1BUFL;
}
void read_spi_flash(uint32_t address, uint16_t bytes, uint8_t* out_buffer) {
    // read is command 0x03 followed by a 24-bit address. the chip
    // then clocks out one byte, and will increment the address pointer
    // and clock out more bytes if you give it more clocks.
    
    // SPI is full-duplex, so we need to make a buffer of 
    // the command, the address, and the number of bytes
    // we want to read as zeros.
    
    // write the command and address
    
    /*    debug_printf("address: %lx\r\n", address);
    __delay_ms(70);
      */          
    assert_chip_select();
    
    transfer_spi_byte(0x03);
    // big-endian byte ordering

    transfer_spi_byte((address >> 16) & 0xff);
    transfer_spi_byte((address >> 8) & 0xff);
    transfer_spi_byte(address & 0xff);

    // now we write zeros and read what comes out
    for (uint16_t i = 0; i < bytes; i++) {
        out_buffer[i] = transfer_spi_byte(0);
    }

    deassert_chip_select();
}
