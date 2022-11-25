#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/io.h>

#define FISRT_BUTTON  PD5
#define SECOND_BUTTON PD6
#define Third_BUTTON  PD7

#define ADDRESS_FOR_LED_DISPLAY 0x42
#define ADDRESS_FOR_LED_BULBS   0x40

#define SDA_PIN 0
#define SCL_PIN 1
#define I2C_ACK 0
#define I2C_NAK 1
#define I2C_PORT PORTC
#define I2C_PIN  PINC
#define I2C_DDR  DDRC

#define LCD_PIN_E  4
#define LCD_PIN_RS 1
#define LCD_PIN_RW 2
#define LCD_CMD    0
#define LCD_DATA   1
#define LCD_DISP_CLEAR   0x01
#define LCD_DISP_OFF     0x08
#define LCD_DISP_ON      0x0C
#define LCD_CURSOR_ON    0x0E
#define LCD_CURSOR_BLINK 0x0F
#define LCD_RETURN_HOME  0x02
#define LCD_ENTRY_MODE   0x06
#define LCD_4BIT_MODE    0x20
#define LCD_8BIT_MODE    0x30
#define LCD_2_ROWS       0x08
#define LCD_FONT_5x8     0x00
#define LCD_FONT_5x10    0x04
#define LCD_POSITION     0x80

#define SDA_HIGH() I2C_DDR &= ~(1<<SDA_PIN)
#define SDA_LOW()  I2C_DDR |=  (1<<SDA_PIN)
#define SCL_HIGH() I2C_DDR &= ~(1<<SCL_PIN)
#define SCL_LOW()  I2C_DDR |=  (1<<SCL_PIN)

#define NUMBER_EFFECTS 3
#define NUMBER_SPEED   3

#define SPEED_LOW     310
#define SPEED_MEDIUM  150
#define SPEED_HIGH    90

#define VALUE_RIGHT_CENTER_LED 16
#define VALUE_LEFT_CENTER_LED  8

#define VALUE_FIRST_LED 1
#define VALUE_LAST_LED  128

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

LedsConfiguration leds_configuration = { 0, 0 };

void I2C_Start(void)
{
    I2C_DDR &= ~((1<<SDA_PIN)|(1<<SCL_PIN));
    _delay_us(5);

    SDA_LOW();
    _delay_us(5);

    SCL_LOW();
}

void I2C_Stop(void)
{
    SDA_LOW();
    _delay_us(5);

    SCL_HIGH();
    _delay_us(3);

    SDA_HIGH();
    _delay_us(2);
}

uint8_t I2C_Write(uint8_t data)
{
    uint8_t i, ack;
    
    for (i = 0; i < 8; i++)
    {
        SCL_LOW();
        _delay_us(2);

        if (data & 0x80)
        {
            SDA_HIGH();
        }
        else
        {
            SDA_LOW();
        }

        _delay_us(4);

        SCL_HIGH();
        _delay_us(5);

        data <<= 1;
    }

    SCL_LOW();
    _delay_us(3);

    SDA_HIGH();
    _delay_us(5);

    SCL_HIGH();
    _delay_us(3);

    ack = I2C_PIN & (1<<SDA_PIN);
    _delay_us(2);

    SCL_LOW();
    _delay_us(5);
    
    return ack;
}

uint8_t I2C_Read(uint8_t ack)
{
    uint8_t i, data = 0;

    for (i = 0; i < 8; i++)
    {
        data <<= 1;

        SCL_LOW();
        _delay_us(5);

        SCL_HIGH();
        _delay_us(5);

        if (I2C_PIN & (1<<SDA_PIN)) 
        {
            data |= 1;
        }
    }

    SCL_LOW();
    _delay_us(2);

    if (!ack)
    {
        SDA_LOW();
    }
    else
    {
        SDA_HIGH();
    }

    _delay_us(3);

    SCL_HIGH();
    _delay_us(5);

    SCL_LOW();
    _delay_us(5);

    return data;
}

void LCD_E_pulse(uint8_t data)
{
    I2C_Write(data | LCD_PIN_E);
    _delay_us(1);

    I2C_Write(data & ~LCD_PIN_E);
    _delay_us(40);
}

void LCD_Send(uint8_t addr, uint8_t d, uint8_t type)
{
    uint8_t up_nibble  = (d & 0xF0);
    uint8_t low_nibble = (d << 4);

    if(type)
    {
        up_nibble  |= LCD_PIN_RS;
        low_nibble |= LCD_PIN_RS;
    }

    I2C_Start();
    I2C_Write(addr);

    I2C_Write(up_nibble);
    LCD_E_pulse(up_nibble);

    I2C_Write(low_nibble);
    LCD_E_pulse(low_nibble);
    I2C_Stop();

    _delay_ms(5);
}

uint8_t InitializationOfLedDisplay(uint8_t addr)
{
    uint8_t res  = 0;
    uint8_t data = 0;

    _delay_ms(16);

    I2C_Start();

    res = I2C_Write(addr);
    if(res != I2C_ACK)
    {
        return 1;
    }

    data = 0x30;
    I2C_Write  (data);
    LCD_E_pulse(data);
    _delay_ms(5);

    I2C_Write  (data);
    LCD_E_pulse(data);
    _delay_us(5);
    
    I2C_Write  (data);
    LCD_E_pulse(data);
    _delay_us(5);

    data = 0x20;
    I2C_Write  (data);
    LCD_E_pulse(data);
    _delay_us(50);
    I2C_Stop();

    LCD_Send(addr, LCD_4BIT_MODE | LCD_2_ROWS | LCD_FONT_5x8, LCD_CMD);
    LCD_Send(addr, LCD_DISP_OFF  , LCD_CMD);
    LCD_Send(addr, LCD_DISP_CLEAR, LCD_CMD);
    LCD_Send(addr, LCD_ENTRY_MODE, LCD_CMD);
    LCD_Send(addr, LCD_DISP_ON   , LCD_CMD);

    return 0;
}

void LCD_SetXY(uint8_t addr, uint8_t x, uint8_t y)
{
    if (y != 0) {
    x += 0x40;
    }

    LCD_Send(addr, (LCD_POSITION | x), LCD_CMD);
}

void LCD_Clear(uint8_t addr)
{
    LCD_Send(addr, LCD_DISP_CLEAR, LCD_CMD);
    _delay_ms(10);
}

void LCD_Print(uint8_t x, uint8_t y, char *str)
{
    LCD_SetXY(ADDRESS_FOR_LED_DISPLAY, x, y);

    while(*str)
    {
        LCD_Send(ADDRESS_FOR_LED_DISPLAY, *str++, LCD_DATA);
    }
}

void DisplayingInformationAboutEffectOnLedDisplay(void)
{
    const uint8_t MAX_NUMBER_SYMBOLS   = 20;
    const uint8_t position_word_effect = 0;
    const uint8_t position_word_speed  = 1;
    const uint8_t variable_position    = 8;

    static uint8_t check_for_effect = NUMBER_EFFECTS + 1;
    if(check_for_effect != leds_configuration.effect)
    {
        char effect[MAX_NUMBER_SYMBOLS];
        sprintf(effect, "%d", leds_configuration.effect);
        LCD_Print(variable_position, position_word_effect, effect);

        check_for_effect = leds_configuration.effect;
    }

    static uint8_t check_for_speed = NUMBER_SPEED + 1;
    if(check_for_speed != leds_configuration.speed)
    {
        char speed[MAX_NUMBER_SYMBOLS];
        sprintf(speed, "%ld", leds_configuration.speed);
        LCD_Print(variable_position, position_word_speed, speed);

        check_for_speed = leds_configuration.speed;
    }
}

uint8_t WriteDataIntoPCF8574(uint8_t adr, uint8_t data)
{
    I2C_Start();
    if(I2C_Write(adr) != I2C_ACK)
    {
        I2C_Stop();
        return 1;
    }

    I2C_Write(~data);
    I2C_Stop();

    return 0;
}

void ProgramDelay(void)
{
    switch(leds_configuration.speed)
    {
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

void StartFirstEffectForLeds(void)
{
    static uint16_t value_led_right = VALUE_RIGHT_CENTER_LED;
    static uint16_t value_led_left  = VALUE_LEFT_CENTER_LED ;

    WriteDataIntoPCF8574(ADDRESS_FOR_LED_BULBS, value_led_right | value_led_left);
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

    WriteDataIntoPCF8574(ADDRESS_FOR_LED_BULBS, value_previous_led | value_next_led);

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

    WriteDataIntoPCF8574(ADDRESS_FOR_LED_BULBS, value_previous_led | value_next_led |
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

void HandlerFirstButton(void)
{
    ++leds_configuration.speed;

    if(leds_configuration.speed > NUMBER_SPEED)
    {
        leds_configuration.speed = 1;
    }
}

void HandlerSecondButton(void)
{
    ++leds_configuration.effect;

    if(leds_configuration.effect > NUMBER_EFFECTS)
    {
        leds_configuration.effect = 1;
    }
}

void HandlerThirdButton(void)
{
    --leds_configuration.speed;

    if(leds_configuration.speed == 0)
    {
        leds_configuration.speed = 3;
    }
}

void FirstButtonScan (void)
{
    static uint8_t shreg;
    shreg <<= 1;

    if( (PIND & (1 << FISRT_BUTTON)) != 0 )
    {
        shreg |= 1;
    }

    if( (shreg & 0x07) == 0x04 )
    {
        HandlerFirstButton();
    }
}

void SecondButtonScan(void)
{
    static uint8_t shreg;
    shreg <<= 1;

    if( (PIND & (1 << SECOND_BUTTON)) != 0 )
    {
        shreg |= 1;
    }

    if( (shreg & 0x07) == 0x04 )
    {
        HandlerSecondButton();
    }
}

void ThirdButtonScan (void)
{
    static uint8_t shreg;
    shreg <<= 1;

    if( (PIND & (1 << Third_BUTTON)) != 0 )
    {
        shreg |= 1;
    }

    if( (shreg & 0x07) == 0x04 )
    {
        HandlerThirdButton();
    }
}

void StartDataInitialization(void)
{
    //Atmega328 Data Initialization
    DDRB = 0;
    DDRC = 0;
    DDRD = 0;
    //Initialization of I2c Bus Interface
    I2C_DDR  &= ~( (1 << SDA_PIN) | (1 << SCL_PIN) );
    I2C_PORT &= ~( (1 << SDA_PIN) | (1 << SCL_PIN) );
    //Initialization of LedDisplay
    InitializationOfLedDisplay(ADDRESS_FOR_LED_DISPLAY);
    //Initialisation Text for LedDisplay
    LCD_Print(0, 0, "Effect: 0");
    LCD_Print(0, 1, "Speed : 0");
}

int main(void)
{  
    StartDataInitialization();
    while(1) {
        //Start Button Scan
        FirstButtonScan();
        SecondButtonScan();
        ThirdButtonScan ();
        //Start Effect for Leds
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
        //Displaying Information about Effect On LedDisplay
        DisplayingInformationAboutEffectOnLedDisplay();
    }
    return 0;
}
