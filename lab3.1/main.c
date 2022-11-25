// #include <avr/io.h>
// #include <util/delay.h>

// int main(void)
// {
//     DDRB = 0xFF;
//     DDRC = 0x00;
//     PORTB = 0b00000000;
//     PORTC = 0b00000001;

//     while (1) {
//         PORTB = ~0b00000110;//1
//         _delay_ms(500);

//         PORTB = 0b00000110;//1
//         _delay_ms(500);
//         PORTB = 0b01011011;//2
//         _delay_ms(500);
//         PORTB = 0b01001111;//3
//         _delay_ms(500);
//         PORTB = 0b01100110;//4
//         _delay_ms(500);
//         PORTB = 0b01101101;//5
//         _delay_ms(500);
//         PORTB = 0b01111001;//6
//         _delay_ms(500);
//         PORTB = 0b00000111;//7
//         _delay_ms(500);
//         PORTB = 0b01111111;//8
//         _delay_ms(500);
//         PORTB = 0b01101111;//9
//         _delay_ms(500);
//         PORTB = 0b00111111;//0
//         _delay_ms(500);

//     }
//     return 0;
// }

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#define OUT_REG_CS PB0

// #define BTN1 PB1
// #define BTN2 PB5
// #define BTN3 PB3

int indx = 0;

static const uint8_t nums[17] = {
    0b00000000, //
    0b00111111, //0
    0b00000110, //1
    0b01011011, //2
    0b01001111, //3
    0b01100110, //4
    0b01101101, //5
    0b01111101, //6
    0b00000111, //7
    0b01111111, //8
    0b01101111, //9
    0b01110111, //A
    0b01111111, //B
    0b00111001, //C
    0b00111111, //D
    0b01111001, //E
    0b01110001 //F
};

// ISR(PCINT1_vect)
// {
//     if (!(PINC & (1 << BTN1))) {
//         index += 1;
//     }
//     if (!(PINC & (1 << BTN2))) {
//         index -= 1;
//     }
//     if (!(PINC & (1 << BTN3))) {
//         index = 0;
//     }
// };

void SPI_MasterInit (void)
{
    // Налаштувати піни MOSI (PB3) і SCK (PB5) як виходи
    DDRB |= (1<<PB3)|(1<<PB5);
    //DDRB &= ~((1<<PB1)|(1<<PB5));
    // Включити SPI, задати режим роботи 0 Master,
    // встановити швидкість fck/16
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
};

void SPI_MasterTransmit(uint8_t cData)
{
    // Початок передачі
    SPDR = cData;
    // Чекати завершення передачі
    while (!(SPSR & (1<<SPIF)));
};

//-----------------------------------------------------------
int main(void)
{
    uint8_t d = 1;
    SPI_MasterInit();
    DDRB |= 1 << OUT_REG_CS; // setup CS

    // DDRC &= ~(1 << DDC4);
    // PORTC |= (1 << PORTC4);
    // DDRC &= ~(1 << DDC5);
    // PORTC |= (1 << PORTC5);
    // PCMSK1 |= (1 << PCINT12);
    // PCMSK1 |= (1 << PCINT13);
    // PCICR |= (1 << PCIE1);
    // sei();

    // PORTB |= 1 << OUT_REG_CS;
    // SPI_MasterTransmit(d);
    // PORTB &= ~(1 << OUT_REG_CS);
    // d = 0b00000110;
    // if (d > 32) d = 1;
    // _delay_ms(200);

    
    //for(int indx = 0;indx <= 10; indx++)
    while(1)
    {
        d = nums[indx];
        PORTB |= 1 << OUT_REG_CS;
        SPI_MasterTransmit(d);
        PORTB &= ~(1 << OUT_REG_CS);
        //d <<= 1;
        //d = nums[i];
        //if (d > 64) d = 1;
        _delay_ms(200);
        if(indx == 16) indx = 0;
        indx++;
    }
    return 0;
}