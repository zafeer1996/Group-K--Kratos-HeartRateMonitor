/*Lab 1 first file - ID number 815009291*/
#include <p18f452.h>

/* Set configuration bits for use with ICD2 */
#pragma config OSC = HS
#pragma config WDT = OFF
#pragma config LVP = OFF

unsigned char counter;

void main (void)
{
    TRISB = 0;
    counter = 1;
    
    while (counter < 16)
    {
        PORTB = counter;
        counter = counter + 1;
        
    }
        
    /*HELLO EVERYONE :D...how are you*/
    
}
