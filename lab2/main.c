#include <avr/io.h>
#include <util/delay.h>
#include <avr/sleep.h> 
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 8000000L
#endif

unsigned char butCount = 0;

int btnDown = 0;
int btnLongDown = 0;

int PDs[8] = {PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7};

ISR(PCINT0_vect, ISR_BLOCK)
{
    int counter = 0;

    
    while (!(PINB & (1 << PB5))){
        counter++;
        _delay_ms(1);
    }

    if (counter >= 2000) {
        btnDown = 1;
    } 
    else if (counter >= 1){
        if(!btnLongDown){
            btnLongDown = 1;

            while (PINB & (1 << PB5)){
                _delay_ms(1);
            }
        } else if (btnLongDown) {
            btnLongDown = 0;
        }
    }
}

void runningFireOn(int port) {
    PORTD |= 1 << port;

    for (int i = 0; i < 200; i++) {
        _delay_ms(1);

        if (btnDown){
            break;
        }
    }
}

void runningFireOff(int port) {
    PORTD &= ~(1 << port);

    for (int i = 0; i < 200; i++) {
        _delay_ms(1);

        if (btnDown){
            break;
        }
    }
}


void jonsonCounterOn(){
    
    for (int i = 0; i <= 7; i++){
        if (btnDown){
            break;
        }

        runningFireOff(PDs[i]);
    }
}

void jonsonCounterOff(){
    
    for (int i = 0; i <= 7; i++){
        if (btnDown){
            break;
        }

        runningFireOn(PDs[i]);
    }
}


void runningFire(){
    PORTD = 0; 


    while (1) {
        if (btnDown){
            btnDown = 0;

            break;
        }

        for (int i = 0; i <= 7; i++){
            runningFireOn(PDs[i]);

            if (btnDown){
                break;
            }

            PORTD &= ~(1 << PDs[i]);
        }
    }
}

void runningShadow(){
    PORTD = 0xFF; 

 
    while (1) {
        if (btnDown){
            btnDown = 0;

            break;
        }

        for (int i = 0; i <= 7; i++){
            runningFireOff(PDs[i]);

            if (btnDown){
                break;
            }

            PORTD |= 1 << PDs[i];
        }
    }
}

void runningCounterJohnson(){
    PORTD = 0; 

    while (1){
        if (btnDown){
            btnDown = 0;

            break;
        }

        jonsonCounterOff();

        jonsonCounterOn();
    }
}


void run() {
    DDRD = 0xFF;
    DDRB &= ~(1<<PB5);
    PCMSK0 |= (1<<PCINT5);
    PCICR |= (1<<PCIE0);

    sei();

    while(1){
        runningShadow();

        runningCounterJohnson();

        runningFire();
        
    }
}

void runningFire3()
{

    //while (1)
    {
        unsigned char i;
        PORTD = 0b00000001;
        for(i=0; i<=7; i++)
        {
            // if (butCount > 1) {
            //     PORTD = 0b00000000;
            //     break;
            // }
            PORTD = (1<<i);
            //PORTD <<= 1;
            _delay_ms(500);
            // PORTD |= (1<<i);
            // _delay_ms(500);
            // PORTD &= ~(1<<i);
            // _delay_ms(500);

        }
        PORTD = 0x00;
    }

}

void runningFire2()
{
        PORTD = 0x01;

        PORTD |= 1 << PD0; // включити світлодіод  
        PORTD |= 1 << PD1; // включити світлодіод  
        _delay_ms(100); // затримка  
        PORTD &= ~(1 << PD0); // виключити світлодіод 
        PORTD &= ~(1 << PD1); // виключити світлодіод  
        _delay_ms(400);  

        PORTD |= 1 << PD2; // включити світлодіод 
        PORTD |= 1 << PD3; // включити світлодіод  
        _delay_ms(100); // затримка  
        PORTD &= ~(1 << PD2); // виключити світлодіод 
        PORTD &= ~(1 << PD3); // виключити світлодіод   
        _delay_ms(400);  
         
        PORTD |= 1 << PD4; // включити світлодіод 
        PORTD |= 1 << PD5; // включити світлодіод   
        _delay_ms(100); // затримка  
        PORTD &= ~(1 << PD4); // виключити світлодіод 
        PORTD &= ~(1 << PD5); // виключити світлодіод   
        _delay_ms(400);  
        
        PORTD |= 1 << PD6; // включити світлодіод
        PORTD |= 1 << PD7; // включити світлодіод    
        _delay_ms(100); // затримка  
        PORTD &= ~(1 << PD6); // виключити світлодіод 
        PORTD &= ~(1 << PD7); // виключити світлодіод   
        _delay_ms(400);  


        PORTD = 0x00;
        

}

void scan_key(void)
{
    static uint8_t shreg;
    shreg <<= 1;
    if((PINB & (1 << PB1)) != 0) {
        shreg |= 1;
    }
    // push key
    if((shreg & 0x07) == 0x04) {
        if (butCount == 0) {
            runningFire3();
            butCount = 1;
            return;
        }
        if (butCount == 1) {
            runningFire2();
            butCount = 0;
            return;
        }
        
    
    }
    
    // up key
    if((shreg & 0x0F) == 0x03) {
        //butCount = 0;
    }
}


int main(void)
{

    // DDRD |= 1 << PD0; 
    // DDRD |= 1 << PD1;
    // DDRD |= 1 << PD2;
    // DDRD |= 1 << PD3; 
    // DDRD |= 1 << PD4; 
    // DDRD |= 1 << PD5; 
    // DDRD |= 1 << PD6;  
    // DDRD |= 1 << PD7;      

    // //DDRD = 0x01;
    // // DDRD = 0xFF;
    // // DDRB = 0x00;
    // // PORTB = 0x01;

    // // while (1) { 
    // //     if(PINB & 0x01){ //button is not pressed
    // //         //running_fire();
    // //         PORTD = 0x00; // виключити світлодіод
    // //     } else {
    // //         //running_fire2();
    // //         //PORTD = 0x01; // включити світлодіод
    // //         PORTD = 0xFF; // включити all світлодіод
    // //     }
    // // } 

 
    //     DDRD = 0xFF;
    //     DDRB = 0x00;
    //     PORTD = 0b00000000;
    //     PORTB = 0b00000001; //PB0
    //     while(1)
    //     {
    //         // if(!(PINB & 0b00000011)) {
    //         //     if(butCount < 1) {
    //         //         running_fire();
    //         //         butCount++;
    //         //     } else 
    //         //     if (butCount > 2 && butCount < 5) {
    //         //         running_fire2();
    //         //         //PORTD = 0xFF;
    //         //         butCount++;
    //         //     } else {//if (butCount < 3) {
    //         //         //running_fire2();
    //         //         PORTD = 0x00;
    //         //         butCount = 0;
    //         //     // } else if (butCount < 10) {
    //         //     //     // butCount++;
    //         //     }
    //         // } else {
    //         //     // if(butCount > 10) {
    //         //     //      butCount = 0;
    //         //     // } else {
    //         //     //    PORTD = 0b00000000;
    //         //     // }
                
    //         // }

    //         // 10 ms thread ---------
    //         scan_key();
    //         _delay_ms(10);
    //     }


    //scan_key();
    _delay_ms(10);
    run();


    return 0;
}


   