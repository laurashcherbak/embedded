#include <avr/io.h>
#include <util/delay.h>
#define LED_PIN PB5 // вбудований в Arduino nano світлодіод

static const char symbol_table[2][5] = {
    {'.', '.', '.', 0, 0}, {'-', '-', '-', 0, 0}
};

void morzo_sent_symbol(char sym)
{
    uint8_t index, i;
    if (sym == 's'){
        index = 0;
    } else {
        index = 1;
    }

    i = 0;
    while(symbol_table[index][i] != 0) {
        PORTB |= 1 << LED_PIN; // включити світлодіод
        if (symbol_table[index][i] == '.') {
           _delay_ms(200); 
        } else {
            _delay_ms(600);
        }
        PORTB &= ~(1 << LED_PIN); // виключити світлодіод
        _delay_ms(500);
        i++;
    }
}

int main(void)
{
    DDRB |= 1 << LED_PIN;
    while (1) {
 /*       for (int i=0; i<3; i++) {
            PORTB |= 1 << LED_PIN; // включити світлодіод
            _delay_ms(200); // затримка
            PORTB &= ~(1 << LED_PIN); // виключити світлодіод
            _delay_ms(500);
        }

        for (int i=0; i<3; i++) {
            PORTB |= 1 << LED_PIN; // включити світлодіод
            _delay_ms(600); // затримка
            PORTB &= ~(1 << LED_PIN); // виключити світлодіод
            _delay_ms(500);
        }

        for (int i=0; i<3; i++) {
            PORTB |= 1 << LED_PIN; // включити світлодіод
            _delay_ms(200); // затримка
            PORTB &= ~(1 << LED_PIN); // виключити світлодіод
            _delay_ms(500);
        }
        */

       morzo_sent_symbol('s');
       morzo_sent_symbol('o');
       morzo_sent_symbol('s');
        _delay_ms(2000); 

    }
    return 0;
}