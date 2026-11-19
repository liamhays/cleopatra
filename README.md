# cleopatra
Cleopatra (the Caesar Seeker) is a PIC24 application that directs you
to the nearest Little Caesar's pizza restaurant, using an offline
database and a GPS module for live heading and distance updates. The
Caesar Seeker is also my EE 3051 final project.


## Hardware
The offline database is stored in SPI flash (I used an [Adafruit
W25Q16 SPI flash breakout](https://www.adafruit.com/product/5635)). I
used a generic 16x2 LCD (the free version of the XC16 compiler cannot
built u8g2 for graphic displays) and a generic u-blox M7 GPS receiver,
programmed to send RMC and GGA NMEA messages at 9600 baud over its
UART. One of these (I don't remember which) is not enabled by
default. The actual microcontroller board is the PIC24FJ64GU205
Curiosity, provided with the class.

## Database
The SPI flash holds a extremely rudimentary database of Little
Caesar's locations across America, assembled using the `get_stores.py`
and `stores_to_binary.py` scripts. `get_stores.py` relies on
`ZIP_Locale_Detail.xls` from the United States Postal Service
(available [here](https://postalpro.usps.com/ZIP_Locale_Detail)),
which is a list of all ZIP codes in the US. This file is included in
the repository for convenience, but it will age over time and become
inaccurate. `get_stores.py` then prompts the Little Caesar's nearest
location finder API and saves unique store data as JSON
files. `stores_to_binary.py` takes out the street address, location
number, latitude, and longitude from each of these files and puts it
in a binary file. You can write this file to the flash using a flash
programmer like
[pico-serprog](https://github.com/stacksmashing/pico-serprog) and
`flashrom`, but you may need to extend the binary file to the size of
the flash using `truncate` or similar.

**Run `get_stores.py` at your own risk!** It is a website scraper and
while it does not put significant load on the server, it looks like
automated traffic (because it is). 

## Software
The only external software component in Cleopatra is
[minmea](https://github.com/kosma/minmea), a one-file NMEA parser
library. minmea is included as a submodule in this repository, but you
will need to make one change to get XC16 and its C standard library to
compile minmea correctly: in `minnmea_gettime`, change th eline

```c
time_t timestamp = timegm(&tm);
```

to

```c
time_t timestamp = 0;
```

XC16's standard library evidently does not include many or all of
`time.h`. This modification doesn't affect any minmea behavior that
the Caesar Seeker relies on.

## Pins
### LCD
| LCD pin | PIC24 pin |
|---------|-----------|
| D4-D7   | RA7-RA10  |
| RS      | RB8       |
| EN      | RB9       |

### SPI for flash
| SPI pin | PIC24 pin |
|---------|-----------|
| MOSI    | RC3       |
| MISO    | RC4       |
| SCK     | RC8       |
| CS      | RB7       |

### UART1 for GPS
RX on RC5.

### UART2 for debug `printf`
TX on RC9, RX on RA11, but no code uses RX. These pins are wired to
the Curiosity debugger's builtin USB-to-UART converter.

### User button
Button connects RA11 to ground when it's pressed.


## Usage
Assemble the hardware, flash the SPI chip, and turn on the Caesar
Seeker. Once it gets a GPS fix, it will scan the database for the
nearest location, then direct you to that location. Expect some
calculation error in the distance and heading caused by the nonperfect
symmetry of the Earth, GPS accuracy, and the low precision of the
PIC24's emulated floating point.
