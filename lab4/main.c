#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/io.h>

#define MAX_NUMBER_SYMBOLS_IN_COMMAND 255
#define MAX_NUMBER_SYMBOLS_IN_ARRAY   100

#define BAUD_RATE 9600

#define OUT_REG_CS PB0
#define MOSI       PB3
#define SCK        PB5

#define BUTTON PD7

#define NUMBER_COMMANDS 4

#define NUMBER_EFFECTS 3
#define NUMBER_SPEED   3

#define NOTATION 10

#define SPEED_LOW     310
#define SPEED_MEDIUM  150
#define SPEED_HIGH    90

#define VALUE_RIGHT_CENTER_LED 16
#define VALUE_LEFT_CENTER_LED  8

#define VALUE_FIRST_LED 1
#define VALUE_LAST_LED  128

#define HELP_COMMAND_STRING "Commands: \r\n" \
                            "--- help\r\n" \
                            "--- info\r\n" \
                            "--- effect\r\n" \
                            "--- speed\r\n\r\n"

enum Commands
{
    COMMANDS_HELP,
    COMMANDS_INFORMATION,
    COMMANDS_EFFECT_SELECTION,
    COMMANDS_SPEED_CHANGE
};

enum Effects
{
    FIRST_EFFECT = 1,
    SECOND_EFFECT,
    THIRD_EFFECT
};

enum Speed
{
    LOW = 1,
    MEDIUM,
    HIGH
};

typedef struct LedsConfiguration
{
     uint32_t speed;
     uint8_t  effect;
} LedsConfiguration;


LedsConfiguration leds_configuration;

const char *commands[NUMBER_COMMANDS];

const uint32_t speeds_numbers[NUMBER_SPEED];

const uint32_t speeds_numbers[NUMBER_SPEED];

const char *commands[NUMBER_COMMANDS];

LedsConfiguration leds_configuration = { 1, 1 };

const char *commands[NUMBER_COMMANDS] = 
                {
                    "help",
                    "info",
                    "effect",
                    "speed"
                };

const uint32_t speeds_numbers[NUMBER_SPEED] =
                {
                    SPEED_LOW,
                    SPEED_MEDIUM,
                    SPEED_HIGH
                };

void ProgramDelay(void)
{
    switch(leds_configuration.speed) {
        case LOW:
            _delay_ms(SPEED_LOW);
            break;
        case MEDIUM:
            _delay_ms(SPEED_MEDIUM);
            break;
        case HIGH:
            _delay_ms(SPEED_HIGH);
            break;
        default:
            _delay_ms(0);
    }
}

void UsartWritingStringInTerminal(const char *string)
{
    while(*string) {
        //Writing Symbol In Terminal
        //Start sending data
        UDR0 = *string++;
        //Wait for complete sending data
        while ( !(UCSR0A & (1 << UDRE0)) ) {}
    }
}

bool SpeedProcessingForLeds(const char *speed_string)
{
    uint8_t speed_number = atoi(speed_string);
    if( ('0' > *speed_string) || (*speed_string > '9') ||
        ( 0  >  speed_number) || ( speed_number >  3) )
    {
        UsartWritingStringInTerminal("Incorrect speed!\r\n\r\n");
        UsartWritingStringInTerminal("Enter the speed(1-3): ");
        return false;
    }    
    leds_configuration.speed = speed_number;
    return true;
}

void StringClearing(char *string, const uint8_t size)
{
    for(uint8_t i = 0; i < size; ++i) string[i] = '\0';
}

void Atmega328DataInitialization(void)
{
    //Init Spi
    DDRB = (1 << OUT_REG_CS) | (1 << MOSI) | (1 << SCK );
    SPCR = (1 << SPE       ) | (1 << MSTR) | (1 << SPR0);
    //
    DDRC = 0;
    DDRD = 0;
}

void StartSendDataLeds(uint8_t data)
{
    PORTB |= 1 << OUT_REG_CS;    
    //Start sending data
    SPDR = data;    
    //Wait for complete sending data
    while (!(SPSR & (1<<SPIF)));
    PORTB &= ~(1 << OUT_REG_CS);
}

bool EffectProcessingForLeds(const char *effect_string)
{
    uint8_t effect_number = atoi(effect_string);
    if( ('0' > *effect_string) || (*effect_string > '9') ||
        ( 0  >  effect_number) || ( effect_number >  3) )
    {
        UsartWritingStringInTerminal("Incorrect effect!\r\n\r\n");
        UsartWritingStringInTerminal("Enter the effect(1-3): ");
        return false;
    }
    leds_configuration.effect = effect_number;
    return true;
}

void StartFirstEffectForLeds(void)
{
    static uint16_t value_led_right = VALUE_RIGHT_CENTER_LED;
    static uint16_t value_led_left  = VALUE_LEFT_CENTER_LED ;
    StartSendDataLeds(value_led_right | value_led_left);
    value_led_right = value_led_right << 1;
    value_led_left  = value_led_left  >> 1;
    if( (value_led_left  < VALUE_FIRST_LED) && 
        (value_led_right > VALUE_LAST_LED) )
    {
        value_led_right = VALUE_RIGHT_CENTER_LED;
        value_led_left  = VALUE_LEFT_CENTER_LED ;
    }
    ProgramDelay();
}

void StartSecondEffectForLeds(void)
{
    static uint16_t value_previous_led = VALUE_FIRST_LED;
    static uint16_t value_next_led     = VALUE_FIRST_LED << 1;
    StartSendDataLeds(value_previous_led | value_next_led);
    static bool direction = true;
    if(value_next_led == VALUE_LAST_LED)
    {
        direction = false;
    }
    else if(value_previous_led == VALUE_FIRST_LED)
    {
        direction = true;
    }
    if(direction)
    {
        value_previous_led = value_previous_led << 1;
        value_next_led     = value_next_led     << 1;
    }
    else
    {
        value_previous_led = value_previous_led >> 1;
        value_next_led     = value_next_led     >> 1;
    }
    ProgramDelay();
}

void StartThirdEffectForLeds(void)
{
    static uint16_t value_previous_led = VALUE_FIRST_LED;
    static uint16_t value_next_led     = VALUE_FIRST_LED << 1;
    static uint16_t value_last_led        = VALUE_LAST_LED;
    static uint16_t value_penultimate_led = VALUE_LAST_LED >> 1;
    StartSendDataLeds(value_previous_led | value_next_led |
                      value_last_led     | value_penultimate_led);
    value_previous_led = value_previous_led << 1;
    value_next_led     = value_next_led     << 1;
    value_last_led        = value_last_led        >> 1;
    value_penultimate_led = value_penultimate_led >> 1;
    if( (value_previous_led > VALUE_LAST_LED ) && 
        (value_last_led     < VALUE_FIRST_LED) )
    {
        value_previous_led = VALUE_FIRST_LED;
        value_next_led     = value_previous_led << 1;
        value_last_led        = VALUE_LAST_LED;
        value_penultimate_led = value_last_led >> 1;
    }
    ProgramDelay();
}

void StartEffectForLeds(void)
{
    switch(leds_configuration.effect)
    {
        case FIRST_EFFECT:
        StartFirstEffectForLeds();
        break;
        case SECOND_EFFECT:
        StartSecondEffectForLeds();
        break;
        case THIRD_EFFECT:
        StartThirdEffectForLeds();
        break;
    }
}

void ButtonHandler(void)
{
    ++leds_configuration.speed;
    if(leds_configuration.speed > NUMBER_SPEED)
    {
        leds_configuration.speed = 1;
        ++leds_configuration.effect;
    }

    if(leds_configuration.effect > NUMBER_EFFECTS)
    {
        leds_configuration.speed  = 1;
        leds_configuration.effect = 1;
    }
}

void StartButtonScan(void)
{
    static uint8_t shreg;
    shreg <<= 1;
    if( (PIND & (1 << BUTTON)) != 0 )
    {
        shreg |= 1;
    }
    if( (shreg & 0x07) == 0x04 )
    {
        ButtonHandler();
    }
}

uint8_t UsartReadingSymbolFromTerminal(void)
{
    //Wait for complete sending data
    while ( !(UCSR0A & (1 << RXC0)) )
    {
        StartButtonScan    ();
        StartEffectForLeds ();
    }    
    //Return received data
    return UDR0;
}

bool UsartReadingStringFromTerminal(char *string, uint8_t *size, const char *message)
{
    char symbol = UsartReadingSymbolFromTerminal();
    if(symbol == '\r' || symbol == '\n')
    {
        if(strlen(string) != 0)
        {
            return true;
        }
        UsartWritingStringInTerminal(message);
    }
    else
    {
        string[(*size)++] = symbol;
    }
    return false;
}

void UsartInterfaceInitialization(uint32_t baud_rate)
{
    //Speed calculation(U2X = 0)
    UBRR0H = (uint8_t)((F_CPU / (16 * baud_rate) - 1) >> 8);
    UBRR0L = (uint8_t) (F_CPU / (16 * baud_rate) - 1)      ;
    UCSR0A = 0;
    //Turn on the transmitter and receiver
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    //Setting the data transmission/recieve frame
    //8 data bits, 1 stop bit, no parity check
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void ToProcessCommandHelp(void)
{
    UsartWritingStringInTerminal(HELP_COMMAND_STRING);
}

void PrintInformationInTerminal(const char *string_number, const char *data)
{
    UsartWritingStringInTerminal(data);
    UsartWritingStringInTerminal(string_number);
    UsartWritingStringInTerminal("\r\n");
}

void ToProcessCommandInformation(void)
{
    char string_number[MAX_NUMBER_SYMBOLS_IN_ARRAY];
    itoa(leds_configuration.speed, string_number, NOTATION);
    PrintInformationInTerminal(string_number, "Speed: ");
    itoa(leds_configuration.effect, string_number, NOTATION);
    PrintInformationInTerminal(string_number, "Effect: ");
    UsartWritingStringInTerminal("\r\n");
}

void ToProcessCommandEffectSelection(void)
{
    UsartWritingStringInTerminal("Enter the effect(1-3): ");
    for( ; ; )
    {
        static uint8_t size = 0;
        static char effect_string[MAX_NUMBER_SYMBOLS_IN_ARRAY];
        bool check = UsartReadingStringFromTerminal(effect_string, &size, "Enter the effect(1-3): ");
        if(check)
        {
            bool successfully = EffectProcessingForLeds(effect_string);
            StringClearing(effect_string, size);
            size = 0;
            if(successfully)
            {
                return;
            }
        }
    }
}

void ToProcessCommandSpeedChange(void)
{
    UsartWritingStringInTerminal("Enter the speed(1-3): ");
    for( ; ; )
    {
        static uint8_t size = 0;
        static char speed_string[MAX_NUMBER_SYMBOLS_IN_ARRAY];
        bool check = UsartReadingStringFromTerminal(speed_string, &size, "Enter the speed(1-3): ");
        if(check)
        {
            bool successfully = SpeedProcessingForLeds(speed_string);
            StringClearing(speed_string, size);
            size = 0;            
            if(successfully)
            {
                return;
            }
        }
    }
}

uint8_t DetermineCommand(const char *command)
{
    for(uint8_t i = 0; i < NUMBER_COMMANDS; ++i)
    {
        if( strcmp(command, commands[i]) == 0)
        {
            return i;
        }
    }
    return NUMBER_COMMANDS + 1;
}

void CommandsProcessing(const char *command)
{
    uint8_t action = DetermineCommand(command);
    switch(action)
    {
        case COMMANDS_HELP:
            ToProcessCommandHelp();
            UsartWritingStringInTerminal("Enter the command: ");
            break;
        case COMMANDS_INFORMATION:
            ToProcessCommandInformation();
            UsartWritingStringInTerminal("Enter the command: ");
            break;
        case COMMANDS_EFFECT_SELECTION:
            ToProcessCommandEffectSelection();
            UsartWritingStringInTerminal("Enter the command: ");
            break;
        case COMMANDS_SPEED_CHANGE:
            ToProcessCommandSpeedChange();
            UsartWritingStringInTerminal("Enter the command: ");
            break;            
        default:
            UsartWritingStringInTerminal("Incorrect command!\r\n\r\n");
            UsartWritingStringInTerminal("Enter the command: ");
            break;
    }
}

void StartTerminalProcess(void)
{
    static uint8_t size = 0;
    static char command[MAX_NUMBER_SYMBOLS_IN_COMMAND];
    bool check = UsartReadingStringFromTerminal(command, &size, "Enter the command: ");
    if(check)
    {
        CommandsProcessing(command);
        StringClearing(command, size);
        size = 0;
    }
}

void StartDataInitialization(void)
{
    UsartInterfaceInitialization(BAUD_RATE);
    CommandsProcessing("help");
    Atmega328DataInitialization();
}

int main(void)
{
    StartDataInitialization();
    for( ; ; )
    {
        StartTerminalProcess();
    }
    return 0;
}
