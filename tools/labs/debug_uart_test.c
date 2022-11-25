#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

#define DEBUG

//source
void printlog(const char *fmt, ...)
{ 
   va_list args;	
   va_start(args, fmt);	
    vfprintf(stdout, fmt, args); //fflush(stdout);
}

//header
#if defined(DEBUG)	
void printlog(const char *fmt, ...);
#define LOG_DEBUG(tg, fmt,...) \
	printlog("D [%s] (%s:%d) " fmt "\n", tg,  __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define	LOG_DEBUG(tg, fmt,...)
#endif


void USART_Init(uint32_t baud)
{
  // Для U2X = 1
  // UBRR = F_CPU / (8 * baudrate) - 1;
  UBRR0H=(F_CPU / (8 * baud) - 1) >> 8;
  UBRR0L= F_CPU / (8 * baud) - 1;

  UCSR0A = 1<<U2X0;
  //Включити передавач та приймач
  UCSR0B=(1 << RXEN0) | (1 << TXEN0);
  //Налаштування фрейму передачі/прийому даних
  //8-біт даних, 1-стоп біти, без перевірки парності
  UCSR0C = (0 << USBS0) | ( 1 << UCSZ01 ) | ( 1 << UCSZ00 );
}

uint8_t USART_Receive(void)
{
    // Чекаємо завершення прийому даних
    while (!(UCSR0A & (1<<RXC0))){ ; }
    // Повертаємо прийняті дані
  return UDR0;
}

void USART_PutChar(uint8_t data)
{
  // Чекаємо завершення передачі даних
  while(!(UCSR0A & (1<<UDRE0))){;}
  // Почати передачу даних
  UDR0=data;
}

int uart_putch(char ch, FILE *stream)
{
  (void) stream;
   if (ch == '\n')
      USART_PutChar('\r');
    USART_PutChar(ch);
 return 0;
}

int uart_getch(FILE *stream)
{
  uint8_t ch;
    ch = USART_Receive();
    // Echo the Output Back to terminal
    USART_PutChar(ch);
  return ch;
}

static FILE uart_stream = FDEV_SETUP_STREAM(USART_PutChar, uart_getch, _FDEV_SETUP_RW);

//=======================================================================================
int main(void)
{
	int dd;

	USART_Init(57600);

	stdout = &uart_stream;
	stdin = &uart_stream;
	
	LOG_DEBUG("MAIN", "Hello!\n");

	printf("Enter data: ");
	scanf("%i", &dd);

	printf("[ res = %i ]\n", dd*dd);



    // mail loop =============
    for(;;) 
    {
	}
  return 0;
}