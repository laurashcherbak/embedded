#include <util/delay.h>
#include <avr/io.h>
#include <stdbool.h>

#define OUT_REG_CS PB2
#define MOSI       PB3
#define SCK        PB5

void BTN1Down(bool *check)
{
    static uint8_t shreg;
    shreg <<= 1;

    if( (PINC & (1 << PC0)) != 0 ) {
        shreg |= 1;
    }

    if( (shreg & 0x07) == 0x04 ) {
        *check = true;
    }
};

void BTN2Down(bool *check)
{
    static uint8_t shreg;
    shreg <<= 1;

    if( (PINC & (1 << PC1)) != 0 ) {
        shreg |= 1;
    }

    if( (shreg & 0x07) == 0x04  ) {
        *check = false;
    }
};

void Max7219SendData(uint8_t address, uint8_t data)
{
    PORTB &= ~(1 << PB2);
    
    SPDR = address;
    while( !(SPSR & (1 << SPIF)) );

    SPDR = data;
    while( !(SPSR & (1 << SPIF)) );

    PORTB |= (1 << PB2);
};

int main(void)
{
    const uint16_t number_column  =   8;
    const uint16_t number_rows    = 128;
    const uint16_t delay          =  50;
    
    //atmega
    DDRB = (1 << OUT_REG_CS) | (1 << MOSI) | (1 << SCK );
    SPCR = (1 << SPE       ) | (1 << MSTR) | (1 << SPR0);
    DDRC = 0;

    Max7219SendData(0x0F, 0); // normal mode
    Max7219SendData(0x09, 0); // no decode
    Max7219SendData(0x0B, 7); // set 8x8 row/col
    Max7219SendData(0x0A, 5); // set intensity 5
    Max7219SendData(0x0C, 1); // power on
    // clear display
    for (uint8_t i = 0; i < 8; i++) {
        Max7219SendData(i + 1, 0);
    }

    bool check = false;

    for( ; ; ) {
        for(uint16_t row = 1; row <= number_rows; ) {
            for(uint16_t column = 1; column <= number_column; ) {
                BTN1Down(&check);
                BTN2Down(&check);

                if(check) {
                    Max7219SendData(column, row);
                    _delay_ms(delay);
                    ++column;
                }
            }

            if(check) {
                row = row << 1;
            }
        }
    }

    return 0;
}
